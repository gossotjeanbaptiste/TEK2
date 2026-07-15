/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** test_command_lifecycle
*/

#include <assert.h>
#include <stdio.h>
#include "../include/server/server_internal.h"
#include "test_command.h"

int finish_ko_command(server_t *server, client_t *client, player_t *player)
{
    (void)server;
    (void)player;
    return enqueue_write(client, "ko\n", 3);
}

static void test_run_take_and_set(void)
{
    args_t args = {0};
    server_t server = {0};
    client_t client = {0};
    player_t player = {0};

    args.width = 1;
    args.height = 1;
    server.args = &args;
    server.world = calloc(1, sizeof(tile_t));
    assert(server.world != NULL);
    player.x = 0;
    player.y = 0;
    server.world[0].resources[RESOURCE_FOOD] = 1;
    assert(run_take(&server, &client, &player, RESOURCE_FOOD) == 1);
    assert(server.world[0].resources[RESOURCE_FOOD] == 0);
    assert(player.inventory[RESOURCE_FOOD] == 1);
    assert(run_set(&server, &client, &player, RESOURCE_FOOD) == 1);
    assert(server.world[0].resources[RESOURCE_FOOD] == 1);
    assert(player.inventory[RESOURCE_FOOD] == 0);
    free(server.world);
}

static void setup_incantation_server(server_t *server, args_t *args,
    player_t *players, client_t *clients)
{
    args->width = 1;
    args->height = 1;
    args->freq = 100;
    server->args = args;
    server->world = calloc(1, sizeof(tile_t));
    assert(server->world != NULL);
    server->players = players;
    server->clients = clients;
    server->nb_players = 1;
    server->nb_clients = 1;
    server->clients[0].fd = 1;
}

static void setup_incantation_player(server_t *server, player_t *player)
{
    player->client_fd = 1;
    player->team_index = 0;
    player->level = 1;
    player->x = 0;
    player->y = 0;
    player->incanting = 0;
    server->players[0] = *player;
}

static void test_run_incantation(void)
{
    args_t args = {0};
    server_t server = {0};
    client_t client = {0};
    player_t player = {0};
    player_t *players = calloc(1, sizeof(player_t));
    client_t *clients = calloc(1, sizeof(client_t));

    setup_incantation_server(&server, &args, players, clients);
    setup_incantation_player(&server, &player);
    server.world[0].resources[RESOURCE_LINEMATE] = 1;
    assert(run_incantation(&server, &client, &server.players[0]) == 1);
    assert(server.players[0].incanting == 1);
    free(server.world);
    free(server.players);
    free(server.clients);
}

static void test_run_fork(void)
{
    server_t server = {0};
    client_t client = {0};
    player_t player = {0};

    player.x = 2;
    player.y = 3;
    player.team_index = 1;
    server.nb_eggs = 0;
    server.next_egg_id = 1;
    assert(run_fork(&server, &client, &player) == 1);
    assert(server.nb_eggs == 1);
    assert(server.eggs[0].x == 2);
    assert(server.eggs[0].y == 3);
    assert(server.eggs[0].team_index == 1);
}

static void setup_egg_spawn_server(server_t *server, args_t *args,
    client_t *clients, player_t *players)
{
    args->width = 5;
    args->height = 5;
    server->args = args;
    server->clients = clients;
    server->players = players;
    server->nb_clients = 1;
    server->nb_players = 0;
    server->nb_eggs = 1;
    server->next_egg_id = 2;
    server->eggs[0].x = 2;
    server->eggs[0].y = 3;
    server->eggs[0].team_index = 0;
    clients[0].fd = 10;
}

static void test_register_player_from_egg(void)
{
    server_t server = {0};
    args_t args = {0};
    client_t clients[1] = {{0}};
    player_t players[1] = {{0}};

    setup_egg_spawn_server(&server, &args, clients, players);
    assert(spawn_player_from_egg(&server, 0, "teamA", 0) == 1);
    assert(server.nb_players == 1);
    assert(server.players[0].x == 2);
    assert(server.players[0].y == 3);
    assert(server.nb_eggs == 0);
}

int main(void)
{
    run_command_tests_part1();
    test_run_take_and_set();
    test_run_incantation();
    test_run_fork();
    test_register_player_from_egg();
    printf("Command tests passed.\n");
    return 0;
}
