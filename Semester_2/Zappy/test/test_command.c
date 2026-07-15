/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** test_command
*/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../include/server/server_internal.h"
#include "test_command.h"

int finish_ok_command(server_t *server, client_t *client, player_t *player)
{
    (void)server;
    (void)player;
    return enqueue_write(client, "ok\n", 3);
}

int finish_silent_command(server_t *server, player_t *player)
{
    (void)server;
    (void)player;
    return 1;
}

void pop_player_command(server_t *server, player_t *player)
{
    (void)server;
    (void)player;
}

static void test_run_forward(void)
{
    server_t server = {0};
    args_t args = {0};
    player_t player = {0};
    client_t client = {0};

    args.width = 5;
    args.height = 5;
    server.args = &args;
    player.dir = DIR_NORTH;
    player.x = 2;
    player.y = 0;
    assert(run_forward(&server, &client, &player) == 1);
    assert(player.x == 2);
    assert(player.y == 4);
    player.dir = DIR_EAST;
    player.x = 4;
    player.y = 4;
    assert(run_forward(&server, &client, &player) == 1);
    assert(player.x == 0);
    assert(player.y == 4);
}

static void test_run_turn(void)
{
    server_t server = {0};
    client_t client = {0};
    player_t player = {0};

    player.dir = DIR_NORTH;
    assert(run_right(&server, &client, &player) == 1);
    assert(player.dir == DIR_EAST);
    assert(run_left(&server, &client, &player) == 1);
    assert(player.dir == DIR_NORTH);
}

static void test_run_connect_nbr(void)
{
    server_t server = {0};
    args_t args = {0};
    client_t client = {0};
    player_t player = {0};
    player_t players[3] = {{0}};

    args.clients_nb = 10;
    server.args = &args;
    server.players = players;
    server.nb_players = 3;
    server.nb_eggs = 1;
    server.eggs[0].team_index = 0;
    server.players[0].team_index = 0;
    server.players[1].team_index = 0;
    server.players[2].team_index = 1;
    player.team_index = 0;
    assert(run_connect_nbr(&server, &client, &player) == 1);
    assert(strncmp(client.write_buffer, "7\n", client.write_pos) == 0);
}

static void test_run_inventory(void)
{
    server_t server = {0};
    client_t client = {0};
    player_t player = {0};

    player.inventory[RESOURCE_FOOD] = 1;
    player.inventory[RESOURCE_LINEMATE] = 2;
    player.inventory[RESOURCE_DERAUMERE] = 3;
    player.inventory[RESOURCE_SIBUR] = 4;
    player.inventory[RESOURCE_MENDIANE] = 5;
    player.inventory[RESOURCE_PHIRAS] = 6;
    player.inventory[RESOURCE_THYSTAME] = 7;
    assert(run_inventory(&server, &client, &player) == 1);
    assert(strstr(client.write_buffer,
            "food 1 linemate 2 deraumere 3 sibur 4 mendiane 5 phiras 6 "
            "thystame 7\n") != NULL);
}

static void setup_look_world(server_t *server, args_t *args)
{
    args->width = 1;
    args->height = 1;
    server->args = args;
    server->world = calloc(1, sizeof(tile_t));
    assert(server->world != NULL);
    server->world[0].resources[RESOURCE_FOOD] = 1;
    server->world[0].resources[RESOURCE_LINEMATE] = 2;
    server->world[0].resources[RESOURCE_DERAUMERE] = 3;
    server->world[0].resources[RESOURCE_SIBUR] = 4;
    server->world[0].resources[RESOURCE_MENDIANE] = 5;
    server->world[0].resources[RESOURCE_PHIRAS] = 6;
    server->world[0].resources[RESOURCE_THYSTAME] = 7;
}

static void test_run_look(void)
{
    args_t args = {0};
    server_t server = {0};
    client_t client = {0};
    player_t player = {0};

    setup_look_world(&server, &args);
    assert(run_look(&server, &client, &player) == 1);
    assert(strstr(client.write_buffer,
            "[food 1, linemate 2, deraumere 3, sibur 4, mendiane 5, "
            "phiras 6, thystame 7]\n") != NULL);
    free(server.world);
}

void run_command_tests_part1(void)
{
    test_run_forward();
    test_run_turn();
    test_run_connect_nbr();
    test_run_inventory();
    test_run_look();
}
