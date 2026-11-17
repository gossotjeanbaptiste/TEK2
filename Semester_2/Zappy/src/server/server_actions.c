/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_actions
*/

#include "../../include/server/server_internal.h"

static int process_one_player(t_server *server, t_player *player, long now)
{
    if (player->incanting) {
        if (player->incant_end_ms <= now) {
            t_client *client = get_client_by_fd(server, player->client_fd);
            if (!client || !finish_incantation(server, client, player))
                return -1;
        }
        return player->command_count > 0;
    }
    if (player->command_count == 0)
        return 0;
    if (player->next_action_time_ms <= now) {
        if (!execute_player_command(server, player))
            return -1;
    }
    return player->command_count > 0;
}

int process_pending_actions(t_server *server)
{
    long now = get_now_ms();
    int has_pending = 0;
    int status;

    for (int i = 0; i < server->nb_players; i++) {
        status = process_one_player(server, &server->players[i], now);
        if (status == -1)
            return 0;
        if (status == 1)
            has_pending = 1;
    }
    return has_pending;
}
