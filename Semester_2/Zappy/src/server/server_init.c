/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_init
*/

#include <stdio.h>
#include "../../include/server/server_internal.h"

static int alloc_server_arrays(t_server *server)
{
    server->clients = calloc(MAX_CLIENTS, sizeof(t_client));
    server->pollfds = calloc(MAX_CLIENTS + 1, sizeof(struct pollfd));
    server->players = calloc(MAX_CLIENTS, sizeof(t_player));
    if (!server->clients || !server->pollfds || !server->players) {
        free(server->clients);
        free(server->pollfds);
        free(server->players);
        return 0;
    }
    return 1;
}

static void init_server_fields(t_server *server, int server_fd, t_args *args)
{
    server->server_fd = server_fd;
    server->nb_clients = 0;
    server->args = args;
    server->world = NULL;
    server->nb_players = 0;
    server->next_player_id = 1;
    server->nb_eggs = 0;
    server->next_egg_id = 1;
    server->game_over = 0;
    server->winning_team[0] = '\0';
    server->last_resource_spawn_ms = 0;
    server->last_food_decay_ms = 0;
}

t_server *init_server(int server_fd, t_args *args)
{
    t_server *server = malloc(sizeof(t_server));

    if (!server)
        return NULL;
    init_server_fields(server, server_fd, args);
    if (!alloc_server_arrays(server)) {
        free(server);
        return NULL;
    }
    server->pollfds[0].fd = server_fd;
    server->pollfds[0].events = POLLIN;
    if (!init_world(server)) {
        free(server->clients);
        free(server->pollfds);
        free(server->players);
        free(server);
        return NULL;
    }
    return server;
}
