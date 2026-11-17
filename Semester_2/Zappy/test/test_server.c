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
    t_client client;
    memset(&client, 0, sizeof(client));

    assert(enqueue_write(&client, "ok\n", 3) == 1);
    assert(client.write_pos == 3);
    assert(memcmp(client.write_buffer, "ok\n", 3) == 0);

    client.write_pos = BUFFER_SIZE - 1;
    assert(enqueue_write(&client, "ab", 2) == 0);
}

static void test_queue_player_command(void)
{
    t_player player;
    memset(&player, 0, sizeof(player));

    assert(queue_player_command(&player, "Forward") == 1);
    assert(player.command_count == 1);
    assert(strcmp(player.commands[0], "Forward") == 0);

    for (int i = 1; i < 10; i++) {
        char command[16];
        snprintf(command, sizeof(command), "Cmd%d", i);
        assert(queue_player_command(&player, command) == 1);
    }
    assert(player.command_count == 10);
    assert(queue_player_command(&player, "Overflow") == 0);
}

static void test_get_tile(void)
{
    t_args args = {0};
    args.width = 3;
    args.height = 2;
    args.freq = DEFAULT_FREQ;
    t_server server = {0};
    server.args = &args;
    server.world = calloc(args.width * args.height, sizeof(t_tile));
    assert(server.world != NULL);

    t_tile *tile = get_tile(&server, 4, -1);
    assert(tile == &server.world[1 * args.width + 1]);
    free(server.world);
}

/* Stubs for symbols in server_command_queue.c that are not part of the minimal
   test set. */
t_player *find_player_by_fd(t_server *server, int client_fd)
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
