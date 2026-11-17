/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_connect
*/

#include <string.h>
#include "../../include/server/server_internal.h"

static int try_set_gui(t_server *server, t_client *client)
{
    client->state = CLIENT_STATE_GUI;
    return enqueue_gui_dump(server, client);
}

static int count_team_slots(t_server *server, int team_index)
{
    int used = 0;

    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].team_index == team_index)
            used++;
    }
    for (int i = 0; i < server->nb_eggs; i++) {
        if (server->eggs[i].team_index == team_index)
            used++;
    }
    return server->args->clients_nb - used;
}

static int try_set_ia(t_server *server, t_client *client, int index,
    const char *name, int team_index)
{
    char line[64];
    int slots;
    int len;

    client->state = CLIENT_STATE_IA;
    client->team_index = team_index;
    strncpy(client->team_name, name, sizeof(client->team_name) - 1);
    client->team_name[sizeof(client->team_name) - 1] = '\0';
    if (!spawn_player_from_egg(server, index, client->team_name, team_index))
        return 0;
    slots = count_team_slots(server, team_index);
    len = snprintf(line, sizeof(line), "%d\n%d %d\n",
        (slots < 0) ? 0 : slots,
        server->args->width, server->args->height);
    if (len < 0)
        return 0;
    return enqueue_write(client, line, len);
}

int try_set_team(t_server *server, int index, const char *name)
{
    t_client *client = &server->clients[index];

    if (strcmp(name, "GRAPHIC") == 0)
        return try_set_gui(server, client);
    for (int i = 0; i < server->args->teams_count; i++) {
        if (strcmp(server->args->teams[i], name) == 0)
            return try_set_ia(server, client, index, name, i);
    }
    return 0;
}
