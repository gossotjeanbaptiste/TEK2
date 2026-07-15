# Zappy Server Architecture Diagrams

Ce document décrit la structure interne du serveur Zappy avec des diagrammes ASCII pour :
- la boucle `poll()`
- la gestion des fichiers de descripteurs (`fds`)
- le cycle de vie d'un client
- le flux des événements entre le moteur de jeu et les clients

---

## 1. Boucle principale (`server_loop`)

```text
+-------------------------------------------------------------+
| server_loop(server)                                         |
|                                                             |
|  while (1)                                                  |
|  {                                                          |
|      timeout = compute_timeout(server)                      |
|      poll(server->pollfds, nb_clients + 1, timeout)         |
|                                                             |
|      if (server socket readable)                            |
|          accept_client(server)                              |
|                                                             |
|      handle_all_clients(server)                             |
|      process_pending_actions(server)                        |
|      process_food_decay(server)                             |
|                                                             |
|      if (server->game_over) break                           |
|  }                                                          |
+-------------------------------------------------------------+
```

### Résumé
- `poll()` attend :
  - nouvelle connexion TCP
  - entrée sur un client existant
  - socket prêt à écrire pour un client avec données en sortie
  - expiration du prochain `next_action_time_ms`
- Les actions de jeu et la décrémentation de nourriture sont traitées après la lecture/écriture réseau.

---

## 2. Gestion des FDs

### 2.1 Structure `pollfds`

```text
pollfds[0] : serveur TCP (accept)
pollfds[1] : client 0
pollfds[2] : client 1
...
pollfds[n] : client n-1
```

### 2.2 Mise à jour des événements

```text
for each client index i:
    server->pollfds[i + 1].events = POLLIN
    if client.write_pos > 0:
        server->pollfds[i + 1].events |= POLLOUT
```

### 2.3 Réponses possibles

```text
if pollfds[0] readable:
    accept_client(server)

for each client:
    if POLLIN set -> handle_client_read(server, i)
    if POLLOUT set -> handle_client_write(server, i)
```

---

## 3. Cycle de vie d'un client

```text
+-----------------+
| nouvelle socket |
+--------+--------+
         |
         v
+----------------------+      accept_client()
| client_t initialisé   |<----------------------+
| state = HANDSHAKE     |                       |
+--------+-------------+                       |
         |                                     |
         v                                     |
+--------------------+        read line        |
| état HANDSHAKE     |------------------------>|
| authentification   |                         |
| équipe ou GRAPHIC  |                         |
+--------+-----------+                         |
         |                                     |
         | success                             |
         v                                     |
+---------------------+      if GRAPHIC        |
| state = GUI         |<------------------+    |
| enqueue_gui_dump()  |                   |    |
+---------------------+                   |    |
         |                                 |    |
         | if IA                          |    |
         +------------------------------> |    |
                                          |    |
                                   +------+------+
                                   | state = IA  |
                                   | spawn_player|
                                   +------+------+   
                                          |          
                                          v          
                                  +---------------+ 
                                  | IN_GAME       |
                                  | player_t live |
                                  +---------------+
```

### État simplifié pour l’équipe IA
- `PENDING` : `CLIENT_STATE_HANDSHAKE`
- `AUTHENTICATED` : `CLIENT_STATE_IA` après `try_set_team()` réussi
- `IN_GAME` : joueur créé et en attente de commandes dans `server->players`

---

## 4. Flux des événements entre le moteur de jeu et les clients

```text
Client IA                         Serveur                       Client GUI
----------                        --------                      -----------
   |                                  |                              |
   |  TCP connect                      |                              |
   |--------------------------------->|                              |
   |                                  |                              |
   |  TEAM_NAME\n / GRAPHIC\n        |                              |
   |--------------------------------->|                              |
   |                                  |                              |
   |     if GRAPHIC:                  |                              |
   |<---------------------------------|                              |
   |     msz / tna / bct*             |                              |
   |                                  |                              |
   |     if IA:                       |                              |
   |<---------------------------------|                              |
   |     CLIENT-NUM 0\n              |                              |
   |                                  |                              |
   |      commands IA                 |                              |
   |--------------------------------->|                              |
   |                                  |                              |
   |  queue_player_command()           |                              |
   |  next_action_time_ms = now + dt   |                              |
   |                                  |                              |
   |                                  |                              |
   |                                  |   poll timeout arrive        |
   |                                  |------------------------------>
   |                                  |                              |
   |                                  | process_pending_actions()     |
   |                                  | execute_player_command()      |
   |                                  |                              |
   |                                  |   if ok/ko -> enqueue_write  |
   |                                  |<-----------------------------|
   |                                  |                              |
   |  send ok/ko / dead               |                              |
   |<---------------------------------|                              |
   |                                  |                              |
   |                                  |  if GUI connected:            |
   |                                  |  notify_gui_position()        |
   |                                  |------------------------------>
   |                                  |                              |
   |                                  |  send ppo / pnw / seg         |
   |                                  |<-----------------------------|
   |                                  |                              |
```

### Détails de l’ordre des événements
1. `process_player_command()` ajoute la commande dans la file du joueur.
2. `compute_timeout()` calcule le plus petit délai `next_action_time_ms` parmi tous les joueurs.
3. `poll()` attend soit un événement réseau soit la fin d’un délai joueur.
4. quand le délai arrive, `process_pending_actions()` exécute la commande.
5. `execute_player_command()` renvoie `ok` ou `ko` et, pour certaines actions, notifie le GUI.
6. `process_food_decay()` est appelé à chaque itération principale pour décrémenter la nourriture.

---

## 5. Schéma de la boucle `poll()` et des conditions des sockets

```text
+-------------------------+
| prepare pollfds         |
| pollfds[0] = server fd  |
| pollfds[i+1] = client i |
| if client.write_pos >0  |
|    events |= POLLOUT    |
+-----------+-------------+
            |
            v
+-------------------------+
| poll(pollfds, n, timeout)|
+-----------+-------------+
            |
            v
+----------------------------+   +------------------------------+
| if pollfds[0].revents      |   | for each client i           |
|    accept_client()         |   |  if POLLIN -> handle_read   |
+----------------------------+   |  if POLLOUT -> handle_write |
                                 +------------------------------+
```

---

## 6. Notes utiles
- `HANDSHAKE` est le seul état où le serveur attend un nom d'équipe.
- `CLIENT_STATE_GUI` ne participe pas à la boucle de jeu IA mais reçoit l'état du monde et les mises à jour graphiques.
- `CLIENT_STATE_IA` doit être lié à un joueur dans `server->players`.
- `enqueue_write()` peut renvoyer `0` si le buffer est plein.
- `handle_client_write()` gère les envois partiels et conserve le reste dans le buffer.

---

## 7. Recommandations pour les équipes
- **Équipe IA** : envoyer les commandes ligne par ligne, puis attendre `ok\n` ou `ko\n`.
- **Équipe GUI** : se connecter avec `GRAPHIC`, parser `msz`, `tna`, `bct`, puis écouter `pnw`, `ppo`, `seg`.
- **Équipe core** : garder la file de chaque `player_t` et le `next_action_time_ms` synchronisés avec `freq`.
