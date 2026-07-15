/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** test_server
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/server/server_internal.h"

static void test_wrap_coord(void)
{
    assert(wrap_coord(0, 1) == 0);
    assert(wrap_coord(3, 4) == 3);
    assert(wrap_coord(-1, 4) == 3);
    assert(wrap_coord(4, 4) == 0);
    assert(wrap_coord(-5, 4) == 3);
    assert(wrap_coord(5, 4) == 1);
}

static void test_resource_index(void)
{
    assert(resource_index("food") == RESOURCE_FOOD);
    assert(resource_index("linemate") == RESOURCE_LINEMATE);
    assert(resource_index("deraumere") == RESOURCE_DERAUMERE);
    assert(resource_index("sibur") == RESOURCE_SIBUR);
    assert(resource_index("mendiane") == RESOURCE_MENDIANE);
    assert(resource_index("phiras") == RESOURCE_PHIRAS);
    assert(resource_index("thystame") == RESOURCE_THYSTAME);
    assert(resource_index("unknown") == -1);
}

static void test_enqueue_write(void)
{
    client_t client;

    memset(&client, 0, sizeof(client));
    assert(enqueue_write(&client, "ok\n", 3) == 1);
    assert(client.write_pos == 3);
    assert(memcmp(client.write_buffer, "ok\n", 3) == 0);
    client.write_pos = BUFFER_SIZE - 1;
    assert(enqueue_write(&client, "ab", 2) == 0);
}

static void test_queue_player_command(void)
{
    player_t player;
    char command[16];

    memset(&player, 0, sizeof(player));
    assert(queue_player_command(&player, "Forward") == 1);
    assert(player.command_count == 1);
    assert(strcmp(player.commands[0], "Forward") == 0);
    for (int i = 1; i < 10; i++) {
        snprintf(command, sizeof(command), "Cmd%d", i);
        assert(queue_player_command(&player, command) == 1);
    }
    assert(player.command_count == 10);
    assert(queue_player_command(&player, "Overflow") == 0);
}

static void test_get_tile(void)
{
    args_t args = {0};
    server_t server = {0};
    tile_t *tile;

    args.width = 3;
    args.height = 2;
    args.freq = DEFAULT_FREQ;
    server.args = &args;
    server.world = calloc(args.width * args.height, sizeof(tile_t));
    assert(server.world != NULL);
    tile = get_tile(&server, 4, -1);
    assert(tile == &server.world[1 * args.width + 1]);
    free(server.world);
}

/* Stubs for symbols in server_command_queue.c that are not part of the minimal
   test set. */
player_t *find_player_by_fd(server_t *server, int client_fd)
{
    (void)server;
    (void)client_fd;
    return NULL;
}

long get_now_ms(void)
{
    return 0;
}

int main(void)
{
    test_wrap_coord();
    test_resource_index();
    test_enqueue_write();
    test_queue_player_command();
    test_get_tile();
    printf("All tests passed.\n");
    return 0;
}
