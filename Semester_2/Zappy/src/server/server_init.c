/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_init
*/

#include <stdio.h>
#include "../../include/server/server_internal.h"

static int alloc_server_arrays(server_t *server)
{
    server->clients = calloc(MAX_CLIENTS, sizeof(client_t));
    server->pollfds = calloc(MAX_CLIENTS + 1, sizeof(struct pollfd));
    server->players = calloc(MAX_CLIENTS, sizeof(player_t));
    if (!server->clients || !server->pollfds || !server->players) {
        free(server->clients);
        free(server->pollfds);
        free(server->players);
        return 0;
    }
    return 1;
}

static void init_server_fields(server_t *server, int server_fd, args_t *args)
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
}

static void spawn_default_egg(server_t *server, int team_index)
{
    egg_t *egg = &server->eggs[server->nb_eggs];

    egg->id = server->next_egg_id;
    server->next_egg_id++;
    egg->x = rand() % server->args->width;
    egg->y = rand() % server->args->height;
    egg->team_index = team_index;
    server->nb_eggs++;
}

static void spawn_team_eggs(server_t *server, int team_index)
{
    for (int c = 0; c < server->args->clients_nb; c++) {
        if (server->nb_eggs >= MAX_CLIENTS)
            break;
        spawn_default_egg(server, team_index);
    }
}

static void init_default_eggs(server_t *server)
{
    if (!server || !server->args)
        return;
    server->nb_eggs = 0;
    for (int t = 0; t < server->args->teams_count; t++)
        spawn_team_eggs(server, t);
}

server_t *init_server(int server_fd, args_t *args)
{
    server_t *server = malloc(sizeof(server_t));

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
    init_default_eggs(server);
    return server;
}
