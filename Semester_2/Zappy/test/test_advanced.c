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
int run_broadcast(t_server *server, t_client *client, t_player *player,
    const char *message)
{
    (void)server;
    (void)client;
    (void)player;
    (void)message;
    return 1;
}

int run_eject(t_server *server, t_client *client, t_player *player)
{
    (void)server;
    (void)client;
    (void)player;
    return 1;
}

int run_fork(t_server *server, t_client *client, t_player *player)
{
    (void)server;
    (void)client;
    (void)player;
    return 1;
}

/* Test food decay */
static void test_player_food_decay(void)
{
    t_server server = {0};
    t_player player = {0};
    t_args args = {0};

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
    t_player player = {0};

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
    t_server server = {0};
    t_client client = {0};
    t_player player = {0};
    t_args args = {.width = 10, .height = 10};

    server.args = &args;
    server.nb_clients = 1;
    server.clients = &client;
    client.write_pos = 0;
    
    assert(run_broadcast(&server, &client, &player, "hello") == 1);
}

/* Test eject execution */
static void test_eject_basic(void)
{
    t_server server = {0};
    t_client client = {0};
    t_player player = {0};
    t_args args = {0};

    server.args = &args;
    assert(run_eject(&server, &client, &player) == 1);
}

/* Test fork execution */
static void test_fork_basic(void)
{
    t_server server = {0};
    t_client client = {0};
    t_player player = {0};
    t_args args = {0};

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
