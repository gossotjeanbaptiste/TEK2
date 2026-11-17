#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../include/server/server_internal.h"

int run_forward(t_server *server, t_client *client, t_player *player);
int run_right(t_server *server, t_client *client, t_player *player);
int run_left(t_server *server, t_client *client, t_player *player);
int run_connect_nbr(t_server *server, t_client *client, t_player *player);
int run_inventory(t_server *server, t_client *client, t_player *player);
int run_look(t_server *server, t_client *client, t_player *player);
int run_take(t_server *server, t_client *client, t_player *player, int res);
int run_set(t_server *server, t_client *client, t_player *player, int res);
int run_incantation(t_server *server, t_client *client, t_player *player);
int run_fork(t_server *server, t_client *client, t_player *player);

int finish_ok_command(t_server *server, t_client *client, t_player *player)
{
    (void)server;
    (void)player;
    return enqueue_write(client, "ok\n", 3);
}

int finish_silent_command(t_server *server, t_player *player)
{
    (void)server;
    (void)player;
    return 1;
}

void pop_player_command(t_server *server, t_player *player)
{
    (void)server;
    (void)player;
}

t_tile fake_tile;

/* Use real server_world implementations when linking with server_world.c. */

static void test_run_forward(void)
{
    t_server server = {0};
    t_args args = {0};
    t_player player = {0};
    t_client client = {0};

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
    t_server server = {0};
    t_client client = {0};
    t_player player = {0};

    player.dir = DIR_NORTH;
    assert(run_right(&server, &client, &player) == 1);
    assert(player.dir == DIR_EAST);

    assert(run_left(&server, &client, &player) == 1);
    assert(player.dir == DIR_NORTH);
}

static void test_run_connect_nbr(void)
{
    t_server server = {0};
    t_args args = {0};
    t_client client = {0};
    t_player player = {0};
    t_player players[3] = {{0}};

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
    t_server server = {0};
    t_client client = {0};
    t_player player = {0};

    player.inventory[RESOURCE_FOOD] = 1;
    player.inventory[RESOURCE_LINEMATE] = 2;
    player.inventory[RESOURCE_DERAUMERE] = 3;
    player.inventory[RESOURCE_SIBUR] = 4;
    player.inventory[RESOURCE_MENDIANE] = 5;
    player.inventory[RESOURCE_PHIRAS] = 6;
    player.inventory[RESOURCE_THYSTAME] = 7;

    assert(run_inventory(&server, &client, &player) == 1);
    assert(strstr(client.write_buffer, "food 1 linemate 2 deraumere 3 sibur 4 mendiane 5 phiras 6 thystame 7\n") != NULL);
}

static void test_run_look(void)
{
    t_args args = {0};
    t_server server = {0};
    t_client client = {0};
    t_player player = {0};

    args.width = 1;
    args.height = 1;
    server.args = &args;
    server.world = calloc(1, sizeof(t_tile));
    assert(server.world != NULL);

    server.world[0].resources[RESOURCE_FOOD] = 1;
    server.world[0].resources[RESOURCE_LINEMATE] = 2;
    server.world[0].resources[RESOURCE_DERAUMERE] = 3;
    server.world[0].resources[RESOURCE_SIBUR] = 4;
    server.world[0].resources[RESOURCE_MENDIANE] = 5;
    server.world[0].resources[RESOURCE_PHIRAS] = 6;
    server.world[0].resources[RESOURCE_THYSTAME] = 7;

    assert(run_look(&server, &client, &player) == 1);
    assert(strstr(client.write_buffer, "[food 1, linemate 2, deraumere 3, sibur 4, mendiane 5, phiras 6, thystame 7]\n") != NULL);
    free(server.world);
}

static void test_run_take_and_set(void)
{
    t_args args = {0};
    t_server server = {0};
    t_client client = {0};
    t_player player = {0};

    args.width = 1;
    args.height = 1;
    server.args = &args;
    server.world = calloc(1, sizeof(t_tile));
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

static void test_run_incantation(void)
{
    t_args args = {0};
    t_server server = {0};
    t_client client = {0};
    t_player player = {0};

    t_player *players = calloc(1, sizeof(t_player));
    t_client *clients = calloc(1, sizeof(t_client));

    args.width = 1;
    args.height = 1;
    args.freq = 100;
    server.args = &args;
    server.world = calloc(1, sizeof(t_tile));
    assert(server.world != NULL);
    server.players = players;
    server.clients = clients;
    server.nb_players = 1;
    server.nb_clients = 1;
    server.clients[0].fd = 1;

    player.client_fd = 1;
    player.team_index = 0;
    player.level = 1;
    player.x = 0;
    player.y = 0;
    player.incanting = 0;
    server.players[0] = player;

    server.world[0].resources[RESOURCE_LINEMATE] = 1;

    assert(run_incantation(&server, &client, &server.players[0]) == 1);
    assert(server.players[0].incanting == 1);
    free(server.world);
    free(server.players);
    free(server.clients);
}

static void test_run_fork(void)
{
    t_server server = {0};
    t_client client = {0};
    t_player player = {0};

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

static void test_register_player_from_egg(void)
{
    t_server server = {0};
    t_args args = {0};
    t_client clients[1] = {{0}};
    t_player players[1] = {{0}};

    args.width = 5;
    args.height = 5;
    server.args = &args;
    server.clients = clients;
    server.players = players;
    server.nb_clients = 1;
    server.nb_players = 0;
    server.nb_eggs = 1;
    server.next_egg_id = 2;
    server.eggs[0].x = 2;
    server.eggs[0].y = 3;
    server.eggs[0].team_index = 0;
    clients[0].fd = 10;

    assert(spawn_player_from_egg(&server, 0, "teamA", 0) == 1);
    assert(server.nb_players == 1);
    assert(server.players[0].x == 2);
    assert(server.players[0].y == 3);
    assert(server.nb_eggs == 0);
}

int main(void)
{
    test_run_forward();
    test_run_turn();
    test_run_connect_nbr();
    test_run_inventory();
    test_run_look();
    test_run_take_and_set();
    test_run_incantation();
    test_run_fork();
    test_register_player_from_egg();

    printf("Command tests passed.\n");
    return 0;
}
