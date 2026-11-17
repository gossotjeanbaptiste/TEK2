/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_loop
*/

#include "../../include/server/server_internal.h"

static int compute_timeout(t_server *server)
{
    int timeout = 126000 / server->args->freq;
    long now = get_now_ms();
    long wait;

    for (int i = 0; i < server->nb_players; i++) {
        t_player *player = &server->players[i];

        if (player->command_count == 0)
            continue;
        wait = player->next_action_time_ms - now;
        if (wait < 0)
            wait = 0;
        if (wait < timeout)
            timeout = (int)wait;
    }
    return timeout;
}

static void run_food_decay(t_server *server)
{
    long now = get_now_ms();
    long interval = 126000 / server->args->freq;

    if (interval <= 0)
        interval = 1;
    if (server->last_food_decay_ms == 0)
        server->last_food_decay_ms = now;
    while (now - server->last_food_decay_ms >= interval) {
        process_food_decay(server);
        server->last_food_decay_ms += interval;
    }
}

static void update_poll_events(t_server *server, int i)
{
    server->pollfds[i + 1].events = POLLIN;
    if (server->clients[i].write_pos > 0)
        server->pollfds[i + 1].events |= POLLOUT;
}

static int handle_one_client(t_server *server, int i)
{
    if (server->pollfds[i + 1].revents & POLLIN) {
        if (!handle_client_read(server, i)) {
            remove_client(server, i);
            return 0;
        }
    }
    if (server->pollfds[i + 1].revents & POLLOUT) {
        if (!handle_client_write(server, i)) {
            remove_client(server, i);
            return 0;
        }
    }
    update_poll_events(server, i);
    return 1;
}

static void handle_all_clients(t_server *server)
{
    for (int i = 0; i < server->nb_clients; i++) {
        if (!handle_one_client(server, i))
            continue;
    }
}

static void flush_pending_writes(t_server *server)
{
    for (int i = 0; i < server->nb_clients; i++) {
        for (int t = 0; t < 100 && server->clients[i].write_pos > 0; t++) {
            if (!handle_client_write(server, i))
                break;
        }
    }
}

static void arm_poll_events(t_server *server)
{
    for (int i = 0; i < server->nb_clients; i++)
        update_poll_events(server, i);
}

void server_loop(t_server *server)
{
    int poll_count;

    while (1) {
        arm_poll_events(server);
        poll_count = poll(server->pollfds, server->nb_clients + 1,
            compute_timeout(server));
        if (poll_count == -1)
            break;
        if (server->pollfds[0].revents & POLLIN)
            accept_client(server);
        handle_all_clients(server);
        respawn_world_resources(server);
        process_pending_actions(server);
        run_food_decay(server);
        if (server->game_over) {
            flush_pending_writes(server);
            break;
        }
    }
}
