/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** test_game
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/server/server_internal.h"

int respawn_world_resources(t_server *server);

static void test_respawn_world_resources(void)
{
    t_server server = {0};
    t_args args = {.width = 2, .height = 2, .freq = 100};
    t_tile *world = calloc(4, sizeof(*world));

    server.args = &args;
    server.world = world;
    server.last_resource_spawn_ms = 0;
    assert(respawn_world_resources(&server) == 1);
    assert(world[0].resources[RESOURCE_FOOD] + world[1].resources[RESOURCE_FOOD]
        + world[2].resources[RESOURCE_FOOD] + world[3].resources[RESOURCE_FOOD] >= 1);
    free(world);
}

static void test_food_decay_and_death(void)
{
    t_server server = {0};
    t_player players[2] = {{0}};
    t_client clients[2] = {{0}};

    server.players = players;
    server.clients = clients;
    server.nb_players = 1;
    server.nb_clients = 1;
    players[0].food = 1;
    players[0].client_fd = 1;
    clients[0].fd = 1;

    process_food_decay(&server);
    assert(players[0].food == 0);
    assert(server.nb_players == 0);
}

static void test_food_decay_uses_inventory(void)
{
    t_server server = {0};
    t_player players[1] = {{0}};
    t_client clients[1] = {{0}};

    server.players = players;
    server.clients = clients;
    server.nb_players = 1;
    server.nb_clients = 1;
    players[0].food = 1;
    players[0].inventory[RESOURCE_FOOD] = 1;
    players[0].client_fd = 1;
    clients[0].fd = 1;

    process_food_decay(&server);
    assert(server.nb_players == 1);
    assert(players[0].food == 2);
    assert(players[0].inventory[RESOURCE_FOOD] == 0);
}

int main(void)
{
    test_respawn_world_resources();
    test_food_decay_and_death();
    test_food_decay_uses_inventory();
    printf("Game management tests passed.\n");
    return 0;
}
