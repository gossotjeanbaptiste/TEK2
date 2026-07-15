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

static void test_respawn_world_resources(void)
{
    server_t server = {0};
    args_t args = {.width = 2, .height = 2, .freq = 100};
    tile_t *world = calloc(4, sizeof(*world));
    int total = 0;

    server.args = &args;
    server.world = world;
    server.last_resource_spawn_ms = 0;
    assert(respawn_world_resources(&server) == 1);
    for (int i = 0; i < 4; i++)
        total += world[i].resources[RESOURCE_FOOD];
    assert(total >= 1);
    free(world);
}

static void test_food_decay_and_death(void)
{
    server_t server = {0};
    player_t players[2] = {{0}};
    client_t clients[2] = {{0}};

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
    server_t server = {0};
    player_t players[1] = {{0}};
    client_t clients[1] = {{0}};

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
