# Zappy AI — Référence des fonctions

Ce document liste et explique **toutes les fonctions** de l'IA Python (`src/ai/`) : leur rôle, leurs paramètres, leur valeur de retour et les effets de bord à connaître.

## Sommaire

- [Vue d'ensemble](#vue-densemble)
- [Flux d'exécution](#flux-dexécution)
- [1. `logger.py`](#1-loggerpy)
- [2. `network.py`](#2-networkpy)
- [3. `toric_navigation.py`](#3-toric_navigationpy-module-autonome-non-branché)
- [4. `zappy_ai.py`](#4-zappy_aipy)
- [5. `main.py`](#5-mainpy-point-dentrée)

---

## Vue d'ensemble

| Fichier | Rôle | Contenu |
|---|---|---|
| `logger.py` | Trace console préfixée par l'identifiant de lignée de l'IA | classe `Logger` |
| `network.py` | Couche socket TCP brute (connexion, lecture ligne par ligne, envoi) | classe `NetworkClient` |
| `toric_navigation.py` | Utilitaires de navigation torique et de lecture de la pyramide de vision | classe `ToricNavigator` (**non utilisée actuellement** par `zappy_ai.py`) |
| `zappy_ai.py` | Cœur de l'IA : authentification, mémoire de jeu, prise de décision, boucle réseau | classe `ZappyAI` |
| `main.py` | Point d'entrée CLI (`argparse`) qui instancie et lance `ZappyAI` | script |

L'IA suit un modèle **1 réponse lue → mémoire mise à jour → 1 action décidée et envoyée**, avec une fenêtre de commandes en vol limitée (`pending_commands < 2`) pour rester réactive sans saturer le buffer serveur.

---

## Flux d'exécution

```text
main.py
  └─ ZappyAI(host, port, team_name)
       ├─ authenticate()               # handshake WELCOME / nom d'équipe / dimensions
       └─ run()                        # boucle infinie
            ├─ network.read_line()     # lit une réponse serveur
            ├─ _is_async_event()       # message/eject/Elevation underway -> ignorés ici
            ├─ _process_response()     # dépile la commande correspondante, met à jour la mémoire
            └─ decide_next_action()    # priorités : manger > forker > miner > s'élever > explorer
                 └─ send_action(cmd)   # envoie la commande, l'ajoute à la file d'attente
```

---

## 1. `logger.py`

### `class Logger`

Petit callable qui préfixe chaque message affiché par l'identifiant de lignée de l'IA (`"1"`, `"1.1"`, `"1.2"`, etc.), ce qui permet de distinguer les logs d'un parent et de ses enfants issus de `Fork` quand plusieurs process tournent dans le même terminal.

#### `__init__(self, ai_id="1")`
- **Rôle** : stocke l'identifiant de lignée utilisé dans le préfixe des logs.
- **Paramètres** : `ai_id` (`str`) — identifiant de l'IA, propagé aux enfants via la variable d'environnement `ZAPPY_AI_ID` (voir `ZappyAI._spawn_new_ai`).
- **Retour** : aucun.

#### `__call__(self, msg)`
- **Rôle** : affiche `msg` précédé de `[IA {ai_id}]`, en forçant le flush immédiat (`flush=True`) pour que les logs de plusieurs process enfants ne s'entremêlent pas de façon incohérente dans le terminal.
- **Paramètres** : `msg` (`str`) — message à afficher.
- **Retour** : aucun.

---

## 2. `network.py`

### `class NetworkClient`

Encapsule la socket TCP et la logique de découpage du flux en lignes (le protocole Zappy est textuel, terminé par `\n`).

#### `__init__(self, host, port, logger=None)`
- **Rôle** : initialise l'état de connexion sans se connecter.
- **Paramètres** : `host` (`str`), `port` (`int`), `logger` (callable optionnel, sinon un `Logger()` par défaut est créé).
- **Attributs posés** : `socket=None`, `buffer=""` (tampon de réception brut).

#### `connect(self)`
- **Rôle** : ouvre la socket TCP (`AF_INET`/`SOCK_STREAM`) vers `(host, port)`.
- **Erreurs** : toute exception de connexion est loguée puis termine le process (`sys.exit(1)`) — pas de retry ici (le retry réseau se fait au niveau de `ZappyAI.authenticate`).
- **Retour** : aucun.

#### `read_line(self)`
- **Rôle** : lit la socket par blocs de 4096 octets et accumule dans `self.buffer` jusqu'à trouver un `\n`, puis retourne la première ligne complète et conserve le reste du buffer pour le prochain appel.
- **Gestion des cas limites** :
  - `recv()` qui lève `ConnectionResetError`/`BrokenPipeError` → log et `sys.exit(0)`.
  - `recv()` qui retourne une chaîne vide (`""`, FIN du pair) → log et `sys.exit(1)`.
  - Si la ligne extraite contient `"dead"` → interprété comme le signal de mort du serveur, log et `sys.exit(0)` immédiat (avant même de retourner la ligne à l'appelant).
- **Retour** : la ligne lue, `strip()`-ée (`str`).

#### `send_command(self, cmd)`
- **Rôle** : envoie `cmd` suivi d'un `\n` encodé en UTF-8.
- **Paramètres** : `cmd` (`str`) — commande brute du protocole (ex. `"Forward"`, `"Take food"`).
- **Erreurs** : `ConnectionResetError`/`BrokenPipeError` → log et `sys.exit(0)`.
- **Retour** : aucun.

#### `close(self)`
- **Rôle** : ferme la socket si elle est ouverte (ignore silencieusement `OSError`) et purge `self.buffer`.
- **Retour** : aucun.
- **Note** : appelé notamment entre deux tentatives d'authentification quand l'équipe est pleine (`ko`), pour repartir sur une socket propre.

---

## 3. `toric_navigation.py` (module autonome, non branché)

### `class ToricNavigator`

> **Note d'implémentation** : cette classe n'est actuellement importée par aucun autre module (`zappy_ai.py` ne l'utilise pas — il recalcule sa propre logique de vision dans `_find_food_direction`). Elle fournit une navigation torique générique (position, direction, avancée avec wrap) et une lecture de la pyramide de vision `Look`, potentiellement destinée à remplacer/factoriser cette logique.

#### `__init__(self, width=0, height=0)`
- **Rôle** : initialise une position `(0, 0)`, orientée `NORTH`, sur une carte de taille `width x height`.
- **Constantes de classe** : `NORTH=0`, `EAST=1`, `SOUTH=2`, `WEST=3`.

#### `set_world_size(self, width, height)`
- **Rôle** : fixe les dimensions de la carte (utilisées par `_wrap`), en forçant des entiers positifs (`max(0, int(...))`).

#### `set_position(self, x, y, direction=None)`
- **Rôle** : repositionne le navigateur ; si `direction` est fourni, l'applique modulo 4 (normalisation défensive).

#### `turn_left(self)`
- **Rôle** : fait pivoter `direction` d'un cran vers la gauche (`(direction - 1) % 4`).

#### `turn_right(self)`
- **Rôle** : fait pivoter `direction` d'un cran vers la droite (`(direction + 1) % 4`).

#### `step_forward(self)`
- **Rôle** : avance d'une case dans la direction courante (`NORTH` → `y -= 1`, `EAST` → `x += 1`, `SOUTH` → `y += 1`, `WEST` → `x -= 1`), puis reboucle les coordonnées via `_wrap`.

#### `_wrap(self, x, y)` *(privé)*
- **Rôle** : applique l'enroulement torique (`x % width`, `y % height`) si les dimensions sont connues (`> 0`).
- **Retour** : tuple `(x, y)` reboublé.

#### `_vision_row_bounds(self, index)` *(privé)*
- **Rôle** : pour un `index` donné dans la liste plate renvoyée par `Look`, calcule à quelle **rangée** de la pyramide de vision il appartient et l'index de la case **centrale** de cette rangée (celle droit devant, à cette profondeur).
- **Principe** : la pyramide de vision a des rangées de tailles `1, 3, 5, 7, ...` ; la rangée `row` commence à l'index `row²` et sa case centrale est à `row² + row`.
- **Retour** : tuple `(first_index_of_row, center_index_of_row)` ; `(0, 0)` si `index <= 0` (case du joueur lui-même).

#### `action_from_vision_index(self, index)`
- **Rôle** : traduit un index de case visible en action de déplacement pour l'atteindre : `"Forward"` si c'est la case centrale de sa rangée, `"Left"` si elle est avant le centre, `"Right"` sinon.
- **Retour** : `str` (`"Forward"`, `"Left"` ou `"Right"`).

#### `best_visible_action(self, look_result, wanted_resources)`
- **Rôle** : parcourt `look_result` (résultat de `Look` déjà décodé en liste) à la recherche de la **première** ressource demandée (dans l'ordre de `wanted_resources`), en ignorant la case 0 (case du joueur), puis retourne l'action pour s'en approcher.
- **Paramètres** : `look_result` (`list`), `wanted_resources` (`str` unique ou `list[str]`, normalisé en liste si besoin).
- **Retour** : `str` (action) ou `None` si rien trouvé / `look_result` invalide.

#### `best_visible_target(self, look_result, resource_name="food")`
- **Rôle** : variante de `best_visible_action` qui retourne directement l'**index** de la première case (hors case 0) contenant `resource_name`, sans le traduire en action.
- **Retour** : `int` (index) ou `None`.

---

## 4. `zappy_ai.py`

### `class ZappyAI`

Contient tout l'état de jeu de l'IA (inventaire, niveau, file de commandes en vol, vision courante) et la logique de décision.

#### `__init__(self, host, port, team_name)`
- **Rôle** : construit l'IA : identifiant de lignée (hérité de la variable d'environnement `ZAPPY_AI_ID`, `"1"` par défaut pour la racine), logger, client réseau, inventaire de départ (`food=10`, le reste à 0), seuils de survie/fork, table `elevation_requirements` (niveaux 1 à 7 : joueurs requis + pierres requises), état initial `"EXPLORING"`, niveau `1`.
- **Attributs clés** :
  - `pending_commands` / `sent_commands_queue` : nombre et liste FIFO des commandes envoyées mais dont la réponse n'est pas encore arrivée (le protocole Zappy répond dans l'ordre d'émission, sauf pour `Incantation`, asynchrone).
  - `last_look_result` / `items_on_current_tile` : dernière vision connue et contenu de la case courante (indice 0 de `last_look_result`).
  - `chase_steps` : file de déplacements (`["Left"/"Right", "Forward"]`) calculée pour rejoindre la nourriture repérée.

#### `authenticate(self, max_retries=5)`
- **Rôle** : réalise le handshake protocolaire Zappy : connexion TCP, attente de `"WELCOME"`, envoi du nom d'équipe, lecture du nombre de slots restants puis des dimensions de la carte.
- **Gestion du `ko`** : si l'équipe est pleine (réponse `"ko"` aux slots), ferme la socket et réessaie après 1 seconde, jusqu'à `max_retries` tentatives ; à la dernière tentative échouée, log d'erreur et `sys.exit(1)`.
- **Retour** : aucun (l'état `slots_available`, `map_width`, `map_height` est mis à jour sur `self`).

#### `send_action(self, cmd)`
- **Rôle** : envoie `cmd` au serveur, incrémente `pending_commands`, l'ajoute à `sent_commands_queue`, logue l'action ; si `cmd` est un déplacement (`Forward`/`Left`/`Right`), invalide la vision courante via `_clear_vision` (la case/le champ de vision change).
- **Retour** : aucun.

#### `parse_response(self, response)`
- **Rôle** : décode une ligne de réponse brute en valeur Python exploitable :
  - `"ok"` → `True`, `"ko"` → `False`.
  - une réponse entre crochets `[...]` (résultat de `Look`/`Inventory`) → `list[str]` des éléments séparés par des virgules (liste vide si le contenu est vide).
  - sinon → la chaîne telle quelle (ex. `"2\n"`, `"Current level: 3"`).
- **Retour** : `bool`, `list[str]` ou `str` selon le cas.

#### `update_memory(self, cmd_sent, decoded_data)`
- **Rôle** : aiguille la réponse déjà décodée vers le bon gestionnaire selon la commande qui l'a provoquée : `Inventory` → `_sync_inventory`, `Look` → `_sync_vision`, `Take <item>` réussi → `_on_take`, `Set <item>` réussi → `_on_set`, `Fork` réussi → `_spawn_new_ai`, `Incantation` → `_on_incantation_result`.
- **Retour** : aucun.

#### `_sync_inventory(self, data)` *(privé)*
- **Rôle** : met à jour `self.inventory` à partir de la réponse à `Inventory`.
- **Tolérance de format** : accepte aussi bien une liste déjà structurée (`["food 10", "linemate 2", ...]`) qu'une chaîne brute `"food 10 linemate 2 ..."` (regroupée par paires nom/quantité), et accepte l'ordre `nom qté` ou `qté nom` selon lequel des deux tokens est numérique.
- **Retour** : aucun ; logue le stock de nourriture si la synchronisation a réellement eu lieu.

#### `_sync_vision(self, tiles)` *(privé)*
- **Rôle** : enregistre le résultat de `Look` (`self.last_look_result`) et le contenu de la case courante (`self.items_on_current_tile = tiles[0].split()`).
- **Garde-fou** : ignore la mise à jour si la case du joueur (première case, sans `"player"`) contient déjà `"food "` dans le tuple — protection contre une resynchronisation qui écraserait un état de nourriture déjà pris en compte par un `Take` en vol.

#### `_on_take(self, item)` *(privé)*
- **Rôle** : après un `Take <item>` réussi, incrémente `self.inventory[item]` et retire une occurrence de `item` de `self.items_on_current_tile` (la case a maintenant une ressource de moins).
- **Retour** : aucun.

#### `_on_set(self, item)` *(privé)*
- **Rôle** : après un `Set <item>` réussi (dépose au sol pour une incantation), décrémente `self.inventory[item]` et ajoute `item` à `self.items_on_current_tile`.
- **Retour** : aucun.

#### `_on_incantation_result(self, data)` *(privé)*
- **Rôle** : traite le résultat **synchrone** de `Incantation` (`ok`/`ko` immédiat) : `False` → échec logué (conditions non réunies), `True` ou une chaîne contenant `"Current level"` → délègue à `_level_up`.
- **Note** : le résultat final réel de l'incantation (le nouveau niveau) arrive en réalité de façon asynchrone plus tard dans la partie et est traité par `_finish_incantation`, pas par ce chemin-ci.

#### `_level_up(self, text)` *(privé)*
- **Rôle** : met à jour `self.level` à partir du texte reçu (`"Current level: N"` → parse `N` après le `:` ; sinon incrémente `self.level` de 1 en secours) et logue la montée de niveau.
- **Retour** : aucun.

#### `_spawn_new_ai(self)` *(privé)*
- **Rôle** : après un `Fork` réussi, lance un nouveau process `main.py` (même interpréteur `sys.executable`, mêmes `-p`/`-n`/`-h`) pour occuper l'œuf pondu, avec `ZAPPY_AI_ID` positionné à `"{ai_id}.{children_spawned}"` pour préfixer ses logs et distinguer la lignée.
- **Choix de conception** : l'enfant est lancé avec `subprocess.Popen` **sans** détacher son groupe de processus — il reste donc rattaché au groupe du parent : un `Ctrl+C` dans le terminal arrête toute la lignée d'un coup, alors que la mort en jeu du parent (réception de `"dead"`) laisse les enfants continuer à jouer.
- **Retour** : aucun ; logue une erreur si `Popen` échoue (`OSError`).

#### `decide_next_action(self)`
- **Rôle** : cœur de la stratégie, appelé quand l'IA a de la marge (`pending_commands < 2`). Ordre de priorité strict :
  1. `_update_state()` (réévalue survie vs exploration selon le stock de food).
  2. Manger la nourriture présente sur la case courante (`"Take food"`), en priorité absolue.
  3. Se reproduire si les conditions de `_should_fork` sont réunies.
  4. En exploration : miner une pierre manquante visible (`_take_needed_stone`) ou tenter une élévation (`_try_elevation`).
  5. Poursuivre une cible de nourriture déjà repérée (`_chase_food`).
  6. Demander l'inventaire après un `Look` (pour recalibrer l'état avant de continuer).
  7. Sinon : `_hunt_food` en mode survie, `_explore` en mode exploration.
- **Retour** : `str` (nom de commande) ou `None`.

#### `_chase_food(self)` *(privé)*
- **Rôle** : si aucun trajet n'est en cours (`chase_steps` vide) mais qu'une vision existe, calcule la direction vers la nourriture (`_find_food_direction`) et prépare la séquence de déplacement (`["Forward"]` si déjà en face, sinon `[direction, "Forward"]`). Dépile ensuite une étape de `chase_steps` à chaque appel.
- **Effet de bord** : invalide la vision (`_clear_vision`) dès qu'une étape est consommée, puisque la case va changer.
- **Retour** : `str` (prochaine étape) ou `None` si rien à poursuivre.

#### `_update_state(self)` *(privé)*
- **Rôle** : bascule l'état entre `"EXPLORING"` et `"SURVIVAL"` selon `self.inventory["food"]` comparé à `CRITICAL_FOOD_THRESHOLD` (8) ; logue le passage en alerte survie au moment de la transition (pas à chaque tick).
- **Retour** : aucun.

#### `_clear_vision(self)` *(privé)*
- **Rôle** : réinitialise `last_look_result` et `items_on_current_tile` — à appeler chaque fois que la case courante ou le champ de vision devient obsolète (déplacement, prise de ressource sur la case en cours de traitement, etc.).
- **Retour** : aucun.

#### `_should_fork(self)` *(privé)*
- **Rôle** : condition de reproduction — vrai seulement en exploration, avec assez de nourriture (`>= FORK_FOOD_THRESHOLD`, 20), sous le quota `MAX_FORKS` (2), et sans `Fork` déjà en vol dans la file d'attente.
- **Retour** : `bool`.

#### `_stone_requirements(self)` *(privé)*
- **Rôle** : extrait, pour le niveau courant, les quantités de pierres requises depuis `elevation_requirements`, en excluant la clé `"nb_players"`.
- **Retour** : `dict[str, int]` (nom de pierre → quantité requise).

#### `_take_needed_stone(self)` *(privé)*
- **Rôle** : si une pierre requise pour l'élévation manque dans l'inventaire et qu'elle est visible sur la case courante, retire mentalement une occurrence de `items_on_current_tile` et retourne la commande de ramassage correspondante.
- **Retour** : `str` (`"Take <pierre>"`) ou `None`.

#### `_try_elevation(self)` *(privé)*
- **Rôle** : vérifie si l'incantation du niveau courant est réalisable **maintenant** — chaque pierre requise doit être couverte par (inventaire + ce qui est déjà déposé sur la case), et le nombre de joueurs présents sur la case doit atteindre `nb_players` requis. Si des pierres manquent encore au sol alors qu'elles sont disponibles en inventaire, retourne un `"Set <pierre>"` ; si tout est en place, retourne `"Incantation"`.
- **Retour** : `str` (`"Set <pierre>"` ou `"Incantation"`) ou `None` si les conditions ne sont pas réunies.

#### `_find_food_direction(self)` *(privé)*
- **Rôle** : parcourt `last_look_result` (en ignorant la case 0) à la recherche de la première case contenant `"food"`, puis calcule sa rangée dans la pyramide de vision (même principe que `ToricNavigator._vision_row_bounds`, réimplémenté localement) pour en déduire `"Forward"` / `"Left"` / `"Right"`.
- **Retour** : `str` ou `None` si aucune nourriture visible.

#### `_hunt_food(self)` *(privé)*
- **Rôle** : comportement en mode `SURVIVAL` sans cible connue : demande occasionnellement l'inventaire (20 % de chance, si non déjà en vol) pour se resynchroniser, sinon lance un `"Look"` ; si une vision existe déjà, avance (`"Forward"`) tout en invalidant cette vision devenue obsolète.
- **Retour** : `str` (`"Inventory"`, `"Look"` ou `"Forward"`).

#### `_explore(self)` *(privé)*
- **Rôle** : comportement en mode `EXPLORING` sans action prioritaire : avance systématiquement juste après un `Look`/`Inventory` en vol (pour ne pas piétiner), sinon tire une action aléatoire pondérée — 60 % `Forward`, 20 % `Look`, 12 % `Left`/`Right`, 8 % `Inventory`.
- **Retour** : `str`.

#### `run(self)`
- **Rôle** : boucle principale du client, exécutée après `authenticate()`. Amorce la boucle avec un `Inventory` puis un `Look`, puis alterne indéfiniment : lit une ligne serveur, sort proprement si c'est `"dead"`, ignore les événements asynchrones (`_is_async_event`), traite la réponse (`_process_response`), et — sauf si une `Incantation` est en vol (le rituel gèle toute action tant qu'il n'est pas résolu) et si la fenêtre de commandes en vol le permet (`pending_commands < 2`) — décide et envoie la prochaine action.
- **Retour** : ne retourne jamais normalement ; se termine via `sys.exit` (déclenché en cascade depuis `network.py`) ou `break` sur réception de `"dead"`.

#### `_is_async_event(self, response)` *(privé)*
- **Rôle** : détecte les messages serveur qui n'ont **pas** été demandés et ne correspondent à aucune commande en file (broadcast `"message ..."`, `"eject ..."`, ou `"Elevation underway"` signalant le gel pendant un rituel), les logue, et indique à l'appelant de les ignorer sans dépiler la file de commandes.
- **Retour** : `bool` (`True` si l'événement a été absorbé).

#### `_process_response(self, response)` *(privé)*
- **Rôle** : si la réponse commence par `"Current level"`, la route vers `_finish_incantation` (résultat asynchrone du rituel) ; sinon, si une commande est en attente, la dépile de `sent_commands_queue`, décrémente `pending_commands`, et transmet la donnée décodée à `update_memory`.
- **Retour** : aucun.

#### `_finish_incantation(self, response)` *(privé)*
- **Rôle** : traite le résultat **réel** (asynchrone) d'une incantation réussie. Comme ce message peut arriver bien après l'`"ok"` immédiat de la commande `Incantation` et alors que d'autres commandes ont pu être empilées entre-temps, il retire spécifiquement `"Incantation"` de `sent_commands_queue` (au lieu de dépiler la tête de file) avant d'appeler `_level_up`.
- **Retour** : aucun.

---

## 5. `main.py` (point d'entrée)

Ce fichier ne définit pas de fonction : c'est un script exécuté directement (`if __name__ == "__main__":`).

- **Arguments CLI** (`argparse`, aide désactivée par défaut et redéfinie manuellement) :
  - `--help` : affiche l'aide.
  - `-p` (obligatoire, `int`) : port du serveur.
  - `-n` (obligatoire, `str`) : nom de l'équipe.
  - `-h` (`str`, défaut `"localhost"`) : machine hôte — attention, ce `-h` **écrase** le `-h`/`--help` habituel d'`argparse`, d'où la désactivation de l'aide automatique (`add_help=False`) et sa redéclaration explicite via `--help`.
- **Déroulé** : instancie `ZappyAI(host, port, team_name)`, appelle `authenticate()` puis `run()` ; un `Ctrl+C` (`KeyboardInterrupt`) est intercepté pour afficher un message de sortie propre plutôt qu'une trace Python brute.
