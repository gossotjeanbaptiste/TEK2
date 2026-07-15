# Guide d'implémentation — Zappy Server (C)

> **Philosophie du guide :** l'objectif est d'avoir un serveur sur lequel la GUI et l'IA peuvent se connecter et communiquer le plus tôt possible. On construit d'abord le squelette réseau, ensuite la logique de jeu.

---

## Phase 1 — Squelette réseau (priorité absolue)

C'est le socle. Sans ça, rien ne peut se connecter.

### 1.1 Parsing des arguments

Traite les flags `-p`, `-x`, `-y`, `-n`, `-c`, `-f` dès le départ. Sans port, le serveur ne peut pas démarrer. Valide les valeurs (port > 0, dimensions > 0, fréquence > 0, au moins un nom d'équipe).

### 1.2 Création du socket serveur

Ouvre un socket TCP, configure-le en `SO_REUSEADDR`, fais le `bind` sur le port passé en argument, puis `listen`. À ce stade le serveur attend des connexions mais ne fait encore rien d'utile.

### 1.3 Boucle `poll` principale

Toute la concurrence repose sur un seul `poll`. Structure ta boucle ainsi :
- Un `pollfd` pour le socket serveur (surveille `POLLIN` pour les nouvelles connexions)
- Un `pollfd` par client connecté (surveille `POLLIN` pour lire, `POLLOUT` quand tu as quelque chose à écrire)

Le `poll` ne doit se débloquer **que** si une activité réseau a lieu ou si une action est prête à être exécutée (timer expiré). Pas d'active waiting.

### 1.4 Accepter les connexions

Quand `POLLIN` arrive sur le socket serveur, appelle `accept`, ajoute le nouveau client à ta liste, envoie-lui immédiatement `WELCOME\n`.

### 1.5 Lecture et écriture bufferisée

Chaque client doit avoir :
- Un **buffer de lecture** entrant (accumule les octets jusqu'à trouver `\n`)
- Un **buffer d'écriture** sortant (accumule ce que tu dois envoyer, vidé quand `POLLOUT` est prêt)

Ne lis et n'écris jamais directement sans passer par ces buffers. C'est ici que tu évites les deadlocks et les reads partiels.

---

## Phase 2 — Handshake et authentification des clients

À ce stade tu peux accepter des connexions. Il faut maintenant identifier qui se connecte.

### 2.1 Réception du nom d'équipe

Après avoir envoyé `WELCOME\n`, attends une ligne du client :
- Si c'est `GRAPHIC` → c'est la GUI, traite-la différemment (pas de slot d'équipe)
- Sinon → c'est un nom d'équipe pour l'IA

### 2.2 Validation du nom d'équipe pour l'IA

Vérifie que le nom correspond à une équipe déclarée avec `-n` et qu'il reste des slots disponibles. Si non → ferme la connexion (ou envoie `ko`).

### 2.3 Réponse au client IA

Envoie dans l'ordre :
1. `CLIENT-NUM\n` (nombre de slots restants dans l'équipe)
2. `X Y\n` (dimensions du monde)

Le client est maintenant un joueur actif. Crée sa structure interne (position aléatoire, direction aléatoire, 10 unités de food = 1260 time units de vie).

### 2.4 Réponse au client GUI

La GUI ne reçoit pas de `CLIENT-NUM` ni de dimensions dans la handshake. Elle s'authentifie puis reçoit un dump de l'état complet du monde (défini par le protocole GUI joint au sujet). Implémente ce dump minimaliste pour l'instant.

> ✅ **Checkpoint :** à ce stade, la GUI et l'IA doivent pouvoir se connecter et être reconnues. Teste avec `nc` ou `telnet` avant d'aller plus loin.

---

## Phase 3 — Monde et ressources

### 3.1 Structure de la carte

Alloue un tableau 2D de tiles de taille `x * y`. Chaque tile contient les quantités de chaque ressource (food, linemate, deraumere, sibur, mendiane, phiras, thystame).

### 3.2 Spawn initial des ressources

Applique la formule `map_width * map_height * density` pour chaque ressource. Distribue-les aléatoirement sur la carte. Assure-toi qu'il y a au moins 1 unité de chaque ressource et de food.

### 3.3 Respawn toutes les 20 time units

Dans ta boucle principale, gère un timer global. Toutes les `20/f` secondes réelles, recalcule et redistribue les ressources manquantes.

---

## Phase 4 — File de commandes et time units

### 4.1 File par joueur

Chaque joueur a une file (FIFO) pouvant contenir jusqu'à **10 commandes**. Au-delà, les commandes sont ignorées. Le client peut envoyer des commandes sans attendre la réponse.

### 4.2 Exécution temporisée

Chaque commande a un coût en time units (`7/f`, `1/f`, `42/f`, `300/f`). Quand tu dépiles une commande pour un joueur, enregistre son timestamp d'expiration. La commande n'est exécutée (et la réponse envoyée) qu'une fois ce délai écoulé. Le joueur est "occupé" pendant ce temps.

### 4.3 Calcul du prochain événement

Pour éviter l'active waiting, calcule le délai avant le prochain événement (prochaine commande prête, prochain respawn, prochain tick de faim) et passe-le comme timeout à `poll`.

---

## Phase 5 — Commandes de base (AI)

Implémente les commandes dans cet ordre de priorité pour tester au plus vite :

### 5.1 Commandes immédiates ou simples
- `Connect_nbr` — répond directement, pas de délai
- `Inventory` — délai `1/f`, renvoie le contenu du joueur
- `Look` — délai `7/f`, calcule le champ de vision selon le niveau

### 5.2 Déplacement
- `Forward` — avance d'une tile dans la direction actuelle (toroïdal : wrap around)
- `Right` / `Left` — tourne la direction du joueur

### 5.3 Interaction avec les ressources
- `Take <object>` — prend un objet sur la tile courante, `ok` ou `ko`
- `Set <object>` — pose un objet sur la tile courante, `ok` ou `ko`

### 5.4 Communication
- `Broadcast <text>` — envoie `message K, text\n` à tous les clients connectés, où K est la direction calculée selon la position relative emetteur/récepteur
- `Eject` — pousse tous les joueurs de la tile dans la direction du joueur éjecteur, détruit les œufs présents

### 5.5 Reproduction
- `Fork` — délai `42/f`, crée un œuf sur la tile courante, ajoute un slot à l'équipe

### 5.6 Mort du joueur
- Gère la décrémentation de food à chaque time unit. Quand food atteint 0, envoie `dead\n` au client et retire le joueur du jeu.

---

## Phase 6 — Incantation (élévation)

C'est la mécanique la plus complexe, à faire en dernier parmi les commandes IA.

### 6.1 Vérification des prérequis

Quand un joueur envoie `Incantation`, vérifie **immédiatement** :
- Nombre de joueurs du même niveau sur la tile
- Quantité de pierres requises sur la tile (table du sujet)

Si les conditions ne sont pas remplies → `ko`.

### 6.2 Gel des joueurs participants

Tous les joueurs impliqués dans l'incantation sont gelés pendant `300/f` secondes. Ils ne peuvent pas exécuter d'autres commandes.

### 6.3 Re-vérification à la fin

À l'expiration du timer, re-vérifie les conditions. Si elles sont toujours remplies → élève tous les joueurs participants, retire les pierres de la tile, envoie `Current level: k`. Sinon → `ko`.

### 6.4 Condition de victoire

Après chaque incantation réussie, vérifie si au moins 6 joueurs d'une même équipe ont atteint le niveau 8. Si oui, annonce la fin de partie.

---

## Phase 7 — Protocole GUI

La GUI reçoit des notifications push du serveur. Implémente les messages progressivement :

- État initial complet au moment de la connexion (toutes les tiles, tous les joueurs, tous les œufs)
- Mise à jour d'une tile quand son contenu change (respawn, prise/dépôt d'objet)
- Position et direction d'un joueur quand il bouge
- Naissance/mort d'un joueur ou d'un œuf
- Début et fin d'incantation
- Broadcast reçu

> Ne renvoie jamais la carte entière pour un changement local. Pousse uniquement la tile ou l'entité modifiée.

---

## Résumé de l'ordre d'implémentation

| Étape | Ce que ça débloque |
|---|---|
| Phase 1 — Réseau + poll | Connexions TCP possibles |
| Phase 2 — Handshake | GUI et IA peuvent s'authentifier ✅ |
| Phase 3 — Monde | La carte existe, les ressources spawn |
| Phase 4 — File + timers | L'exécution temporisée est fonctionnelle |
| Phase 5.1 à 5.3 — Commandes de base | L'IA peut jouer basiquement |
| Phase 5.4 à 5.6 — Comm + Fork + mort | L'IA survit et se reproduit |
| Phase 6 — Incantation | Le jeu peut être gagné |
| Phase 7 — Protocole GUI complet | La GUI affiche tout correctement |

---

## Points de vigilance transversaux

- **Toroïdalité** : toutes les opérations de position doivent utiliser le modulo sur `x` et `y`. Ne jamais sortir de la grille.
- **Calcul du broadcast** : la direction K est calculée en cherchant le chemin le plus court sur le monde sphérique (shortest path toroïdal).
- **`GRAPHIC` est un nom réservé** : refuse toute équipe IA qui tenterait de s'appeler `GRAPHIC`.
- **Single thread** : le serveur est un seul process, un seul thread. Tout passe par `poll`. Aucun `fork`, aucun thread POSIX.
- **Pas de blocking I/O** : tous les sockets doivent être en mode non-bloquant (`O_NONBLOCK`). Gère `EAGAIN` / `EWOULDBLOCK`.
