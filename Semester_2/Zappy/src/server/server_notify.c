/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_notify
*/

#include "../../include/server/server_internal.h"

int notify_gui_position(server_t *server, player_t *player)
{
    char line[BUFFER_SIZE];
    int len;
    client_t *client;

    for (int i = 0; i < server->nb_clients; i++) {
        client = &server->clients[i];
        if (client->state != CLIENT_STATE_GUI)
            continue;
        len = snprintf(line, sizeof(line), "ppo %d %d %d %d\n",
            player->id, player->x, player->y, player->dir);
        if (len < 0 || !enqueue_write(client, line, len))
            return 0;
    }
    return 1;
}

int notify_gui_player_death(server_t *server, int player_id)
{
    char line[BUFFER_SIZE];
    int len;
    client_t *client;

    for (int i = 0; i < server->nb_clients; i++) {
        client = &server->clients[i];
        if (client->state != CLIENT_STATE_GUI)
            continue;
        len = snprintf(line, sizeof(line), "pdi %d\n", player_id);
        if (len < 0 || !enqueue_write(client, line, len))
            return 0;
    }
    return 1;
}

int notify_gui_new_player(server_t *server, player_t *player)
{
    char line[BUFFER_SIZE];
    int len;
    client_t *client;

    for (int i = 0; i < server->nb_clients; i++) {
        client = &server->clients[i];
        if (client->state != CLIENT_STATE_GUI)
            continue;
        len = snprintf(line, sizeof(line),
            "pnw %d %d %d %d %d %s\n",
            player->id, player->x, player->y,
            player->dir, player->level, player->team_name);
        if (len < 0 || !enqueue_write(client, line, len))
            return 0;
    }
    return 1;
}
