# Zappy Server Internal Protocol

Ce document décrit le protocole interne du serveur Zappy : format des buffers, machine à états des clients, gestion des timeouts, et séquence d'authentification.

## 1. Concepts clés

### 1.1 Types principaux
- `server_t` : état global du serveur.
- `client_t` : état d'une connexion socket.
- `player_t` : état d'un joueur IA en jeu.
- `tile_t` : ressources d'une case de la carte.
- `egg_t` : œuf créé par la commande `Fork`.
- `args_t` : paramètres de la simulation (`width`, `height`, `clients_nb`, `freq`, `teams`, ...).

### 1.2 Rôles des connexions
- IA : client qui joue dans le monde Zappy.
- GUI : client qui visualise le monde avec le protocole graphique.

## 2. Buffers

### 2.1 Buffer de lecture
Chaque `client_t` contient :
- `char read_buffer[BUFFER_SIZE];`
- `int read_pos;`

Le serveur lit les données réseau dans `read_buffer + read_pos` puis met à jour `read_pos`.

Le parser interne traite les lignes terminées par `\n` :
- `consume_lines()` scanne le buffer jusqu'à `\n`
- remplace `\n` par `\0`
- appelle `dispatch_line()` pour chaque ligne complète
- décale les octets restants au début du buffer
- conserve les fragments incomplets pour la prochaine lecture

### 2.2 Buffer d'écriture
Chaque `client_t` contient :
- `char write_buffer[BUFFER_SIZE];`
- `int write_pos;`

La fonction `enqueue_write(client, data, len)` :
- vérifie qu'il y a assez d'espace dans `write_buffer`
- copie `len` octets à la fin du buffer
- incrémente `write_pos`

La fonction `handle_client_write()` :
- envoie au socket `client->write_buffer` sur `write_pos`
- si l'envoi envoie `sent` octets, décale le reste
- réduit `write_pos`

## 3. Machine à états des clients

### 3.1 États définis
Dans `client_t` :
- `CLIENT_STATE_HANDSHAKE`
- `CLIENT_STATE_IA`
- `CLIENT_STATE_GUI`

Pour l'équipe IA, on peut interpréter ces états comme :
- `PENDING` = `CLIENT_STATE_HANDSHAKE`
- `AUTHENTICATED` = `CLIENT_STATE_IA` ou `CLIENT_STATE_GUI`
- `IN_GAME` = client IA avec un `player_t` associé dans `server->players`

### 3.2 Transition standard
1. connexion TCP acceptée
2. état initial : `CLIENT_STATE_HANDSHAKE`
3. le client envoie son nom d'équipe (ou `GRAPHIC`)
4. `dispatch_line()` appelle `try_set_team()`
5. si l'équipe est `GRAPHIC` :
   - `client->state = CLIENT_STATE_GUI`
   - réponse : `msz`, `tna`, puis dump complet des cases (`bct`)
6. si le nom correspond à une équipe IA :
   - `client->state = CLIENT_STATE_IA`
   - `client->team_name` et `client->team_index` sont initialisés
   - `spawn_player_from_egg()` crée un joueur à partir d'un œuf existant ou spawn classique
   - réponse : `CLIENT-NUM 0\n`
7. le client IA est alors authentifié et en jeu.

### 3.3 Rejets d'authentification
- si le nom ne correspond à aucune équipe connue, `try_set_team()` retourne 0
- `handle_client_read()` renverra 0 et la connexion sera fermée

## 4. Séquencement d'authentification

### 4.1 Arrivée d'un client
- `accept_client()` crée une entrée `client_t`
- socket en mode non bloquant
- `state = CLIENT_STATE_HANDSHAKE`

### 4.2 Lecture des données de handshake
- `handle_client_read()` lit du socket
- `consume_lines()` extrait chaque ligne complète
- `dispatch_line()` appelle `try_set_team()`

### 4.3 Traitement IA
- `try_set_ia()` :
  - bascule l'état en `CLIENT_STATE_IA`
  - associe la connexion à une équipe
  - appuie sur `spawn_player_from_egg()` pour créer `player_t`
  - envoie `CLIENT-NUM 0\n`

### 4.4 Traitement GUI
- `try_set_gui()` :
  - bascule l'état en `CLIENT_STATE_GUI`
  - envoie l'état initial du monde

## 5. Gestion des commandes IA

### 5.1 File de commandes
Chaque `player_t` a :
- `char commands[10][64];`
- `int command_count;`
- `long next_action_time_ms;`

### 5.2 Ajout d'une commande
`process_player_command(server, client, cmd)` :
- trouve le joueur via `find_player_by_fd(server, client->fd)`
- si le joueur est en incantation, renvoie `ko`
- ajoute la commande dans la file avec `queue_player_command()`
- si c'est la première commande, fixe `next_action_time_ms` à `now + 1000 / freq`

### 5.3 Exécution différée
- le serveur calcule un timeout global via `compute_timeout()` :
  - pour chaque joueur avec des commandes en attente
  - `timeout = next_action_time_ms - now`
- passe ce timeout à `poll()` pour réveiller le serveur au moment du prochain traitement

### 5.4 Exécution d'une commande
- après `poll`, `process_pending_actions()` est appelé
- pour chaque joueur :
  - si `incanting` et `incant_end_ms <= now`, termine l'incantation
  - sinon si `command_count > 0` et `next_action_time_ms <= now`, appelle `execute_player_command()`
- `execute_player_command()` parse et exécute la commande courante
- `finish_ok_command()` ou `finish_silent_command()` retire la commande et met à jour `next_action_time_ms`

### 5.5 Durée d'une commande
- la logique courante fixe toutes les commandes du pipeline à la même durée implicite : `1000 / freq`
- les commandes sont donc déclenchées séquentiellement toutes les `time units` du serveur
- l'incantation utilise une mécanique dédiée pour mettre plusieurs joueurs en attente jusqu'à `incant_end_ms`

## 6. Gestion des timeouts

### 6.1 Timeout réseau
- la boucle principale attend sur `poll()`
- `poll()` gère à la fois l'entrée réseau et la disponibilité d'écriture
- `handle_client_read()` et `handle_client_write()` traitent les I/O sans blocage

### 6.2 Timeout des actions joueurs
- `compute_timeout()` calcule le délai minimum parmi tous les joueurs en attente de commande
- si aucun joueur n'a de commande, `poll()` peut bloquer indéfiniment en attendant un événement réseau
- après `poll()` :
  - traitement I/O client
  - `process_pending_actions()`
  - `process_food_decay()`

### 6.3 Food decay
- l'alimentation est décrémentée dans `process_food_decay(server)` à chaque itération de boucle, après `poll()`
- la mortalité est traitée immédiatement : si `food <= 0`, le joueur est retiré et reçoit `dead\n`

## 7. Protocole de messages

### 7.1 IA
Toutes les commandes sont envoyées ligne par ligne (`\n` séparateur).
- Handshake : `TEAM_NAME\n` ou `GRAPHIC\n`
- Réponse d'IA : `CLIENT-NUM 0\n`
- Réponse commandes : `ok\n` ou `ko\n`
- Parallèlement, le serveur peut envoyer `dead\n` lors de la mort

### 7.2 GUI
Les messages graphiques internes actuellement supportés :
- `msz W H\n`
- `tna team1 team2 ...\n`
- `bct x y q0 q1 q2 q3 q4 q5 q6\n`
- `pnw id x y dir level team\n`
- `ppo id x y dir\n`
- `seg team\n`

### 7.3 Format des buffers
- lecture : accumulation jusqu'au prochain `\n`
- écriture : concaténation dans `write_buffer`
- sortie réseau : envoi partiel accepté, reste décalé vers le début du buffer

## 8. Remarques pour l'équipe IA
- l'IA doit envoyer d'abord son nom d'équipe
- après acceptation, elle reçoit `CLIENT-NUM 0\n`
- envoyer ensuite les commandes IA terminées par `\n`
- la file de commandes limitée à 10 commandes
- pas de timeout explicite d'authentification côté serveur

## 9. Remarques pour l'équipe GUI
- le client GUI peut se connecter avec le token `GRAPHIC`
- il reçoit immédiatement l'état initial du monde via `enqueue_gui_dump()`
- il reçoit ensuite les mises à jour de joueurs et la fin de partie
- les messages `pnw`, `ppo`, `bct`, `tna`, `msz`, `seg` sont les seuls actuellement produits

## 10. Évolutions recommandées
- ajouter un état explicite `IN_GAME` pour l’IA
- enrichir la mise à jour GUI : `plv`, `pin`, `pdi`, `pbc`, `pie`, `pgt`, `pdr`
- documenter et normaliser `CLIENT-NUM` et l'assignation de slots d'équipe
- synchroniser la décroissance de nourriture sur un timer explicite si besoin
