/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_loop
*/

#include "../../include/server/server_internal.h"

static void consider_deadline(long deadline, long now, int *timeout)
{
    long wait;

    if (deadline <= 0)
        return;
    wait = deadline - now;
    if (wait < 0)
        wait = 0;
    if (*timeout == -1 || wait < *timeout)
        *timeout = (int)wait;
}

static int compute_timeout(server_t *server)
{
    int timeout = -1;
    long now = get_now_ms();
    player_t *player;

    for (int i = 0; i < server->nb_players; i++) {
        player = &server->players[i];
        if (player->command_count > 0)
            consider_deadline(player->next_action_time_ms, now, &timeout);
        if (player->incanting)
            consider_deadline(player->incant_end_ms, now, &timeout);
        consider_deadline(player->next_food_decay_ms, now, &timeout);
    }
    if (server->nb_players > 0)
        consider_deadline(server->last_resource_spawn_ms
            + 20000 / server->args->freq, now, &timeout);
    return timeout;
}

static void populate_pollfds(server_t *server)
{
    memset(server->pollfds, 0,
        sizeof(struct pollfd) * (server->nb_clients + 1));
    server->pollfds[0].fd = server->server_fd;
    server->pollfds[0].events = POLLIN;
    for (int i = 0; i < server->nb_clients; i++) {
        server->pollfds[i + 1].fd = server->clients[i].fd;
        server->pollfds[i + 1].events = POLLIN;
        if (server->clients[i].write_pos > 0)
            server->pollfds[i + 1].events |= POLLOUT;
    }
}

static void handle_io_client(server_t *server, int i, int revents)
{
    if (server->clients[i].fd == -1)
        return;
    if (revents & POLLIN)
        if (!handle_client_read(server, i)) {
            remove_client(server, i);
            return;
        }
    if (revents & POLLOUT)
        if (!handle_client_write(server, i))
            remove_client(server, i);
}

static void check_all_poll_events(server_t *server)
{
    int current_nb = server->nb_clients;

    if (server->pollfds[0].revents & POLLIN)
        accept_client(server);
    for (int i = 0; i < current_nb; i++)
        handle_io_client(server, i, server->pollfds[i + 1].revents);
    compact_clients(server);
}

void server_loop(server_t *server)
{
    int poll_count;

    while (1) {
        populate_pollfds(server);
        poll_count = poll(server->pollfds, server->nb_clients + 1,
            compute_timeout(server));
        if (poll_count == -1)
            break;
        check_all_poll_events(server);
        process_food_decay(server);
        process_pending_actions(server);
        respawn_world_resources(server);
    }
}
