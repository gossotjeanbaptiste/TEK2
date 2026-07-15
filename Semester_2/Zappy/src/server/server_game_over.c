/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_game_over
*/

#include <string.h>
#include "../../include/server/server_internal.h"

int notify_gui_game_over(server_t *server, const char *team_name)
{
    char line[BUFFER_SIZE];
    int len;
    client_t *client;

    for (int i = 0; i < server->nb_clients; i++) {
        client = &server->clients[i];
        if (client->state != CLIENT_STATE_GUI)
            continue;
        len = snprintf(line, sizeof(line), "seg %s\n", team_name);
        if (len < 0 || !enqueue_write(client, line, len))
            return 0;
    }
    return 1;
}

static int count_team_level8(server_t *server, int team_index)
{
    int count = 0;

    for (int i = 0; i < server->nb_players; i++)
        if (server->players[i].team_index == team_index &&
            server->players[i].level >= 8)
            count++;
    return count;
}

int check_victory(server_t *server)
{
    if (!server || server->game_over)
        return 0;
    for (int t = 0; t < server->args->teams_count; t++)
        if (count_team_level8(server, t) >= 6) {
            server->game_over = 1;
            strncpy(server->winning_team, server->args->teams[t],
                sizeof(server->winning_team) - 1);
            server->winning_team[sizeof(server->winning_team) - 1] = '\0';
            return notify_gui_game_over(server, server->winning_team);
        }
    return 1;
}
