/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_connect
*/

#include <string.h>
#include <stdio.h>
#include "../../include/server/server_internal.h"

static int try_set_gui(server_t *server, client_t *client)
{
    client->state = CLIENT_STATE_GUI;
    return enqueue_gui_dump(server, client);
}

static int enqueue_ia_init(server_t *server, client_t *client)
{
    char line[BUFFER_SIZE];
    int len;
    int remaining = count_team_eggs(server, client->team_index);

    len = snprintf(line, sizeof(line), "%d\n", remaining);
    if (len < 0 || !enqueue_write(client, line, len))
        return 0;
    len = snprintf(line, sizeof(line), "%d %d\n",
        server->args->width, server->args->height);
    if (len < 0 || !enqueue_write(client, line, len))
        return 0;
    return 1;
}

static int try_set_ia(server_t *server, client_t *client, int index,
    int team_index)
{
    const char *name = server->args->teams[team_index];

    if (count_team_eggs(server, team_index) <= 0)
        return 0;
    client->state = CLIENT_STATE_IA;
    client->team_index = team_index;
    strncpy(client->team_name, name, sizeof(client->team_name) - 1);
    client->team_name[sizeof(client->team_name) - 1] = '\0';
    if (!spawn_player_from_egg(server, index, client->team_name, team_index))
        return 0;
    return enqueue_ia_init(server, client);
}

int try_set_team(server_t *server, int index, const char *name)
{
    client_t *client = &server->clients[index];

    if (strcmp(name, "GRAPHIC") == 0)
        return try_set_gui(server, client);
    for (int i = 0; i < server->args->teams_count; i++)
        if (strcmp(server->args->teams[i], name) == 0)
            return try_set_ia(server, client, index, i);
    return 0;
}
