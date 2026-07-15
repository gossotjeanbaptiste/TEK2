/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_actions
*/

#include "../../include/server/server_internal.h"

static int finish_if_incant_done(server_t *server, player_t *player, long now)
{
    client_t *client;

    if (player->incant_end_ms > now)
        return 1;
    client = get_client_by_fd(server, player->client_fd);
    if (!client || !finish_incantation(server, client, player))
        return -1;
    return 1;
}

static int process_one_player(server_t *server, player_t *player, long now)
{
    if (player->incanting) {
        if (finish_if_incant_done(server, player, now) == -1)
            return -1;
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

int process_pending_actions(server_t *server)
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
