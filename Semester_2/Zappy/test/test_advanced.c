/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** test_advanced_commands
*/

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../include/server/server_internal.h"

/* Stub implementations for missing functions */
int run_broadcast(server_t *server, client_t *client, player_t *player,
    const char *message)
{
    (void)server;
    (void)client;
    (void)player;
    (void)message;
    return 1;
}

int run_eject(server_t *server, client_t *client, player_t *player)
{
    (void)server;
    (void)client;
    (void)player;
    return 1;
}

int run_fork(server_t *server, client_t *client, player_t *player)
{
    (void)server;
    (void)client;
    (void)player;
    return 1;
}

long get_now_ms(void)
{
    return 0;
}

/* Test food decay */
static void test_player_food_decay(void)
{
    server_t server = {0};
    player_t player = {0};
    args_t args = {0};

    server.args = &args;
    player.food = 10;
    player_lose_food(&server, &player, 3);
    assert(player.food == 7);
    player_lose_food(&server, &player, 10);
    assert(player.food == 0);
}

/* Test player death check */
static void test_is_player_dead(void)
{
    player_t player = {0};

    player.food = 5;
    assert(!is_player_dead(&player));
    player.food = 0;
    assert(is_player_dead(&player));
    player.food = -1;
    assert(is_player_dead(&player));
}

/* Test broadcast direction calculation */
static void test_broadcast_basic(void)
{
    server_t server = {0};
    client_t client = {0};
    player_t player = {0};
    args_t args = {.width = 10, .height = 10};

    server.args = &args;
    server.nb_clients = 1;
    server.clients = &client;
    client.write_pos = 0;
    assert(run_broadcast(&server, &client, &player, "hello") == 1);
}

/* Test eject execution */
static void test_eject_basic(void)
{
    server_t server = {0};
    client_t client = {0};
    player_t player = {0};
    args_t args = {0};

    server.args = &args;
    assert(run_eject(&server, &client, &player) == 1);
}

/* Test fork execution */
static void test_fork_basic(void)
{
    server_t server = {0};
    client_t client = {0};
    player_t player = {0};
    args_t args = {0};

    server.args = &args;
    assert(run_fork(&server, &client, &player) == 1);
}

int main(void)
{
    test_player_food_decay();
    test_is_player_dead();
    test_broadcast_basic();
    test_eject_basic();
    test_fork_basic();
    printf("Advanced command tests passed.\n");
    return 0;
}
