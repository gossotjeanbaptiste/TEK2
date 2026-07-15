/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_game
*/

#include <stdlib.h>
#include "../../include/server/server_internal.h"

static const float resource_density[RESOURCE_COUNT] = {
    [RESOURCE_FOOD] = 0.50f,
    [RESOURCE_LINEMATE] = 0.30f,
    [RESOURCE_DERAUMERE] = 0.15f,
    [RESOURCE_SIBUR] = 0.10f,
    [RESOURCE_MENDIANE] = 0.10f,
    [RESOURCE_PHIRAS] = 0.08f,
    [RESOURCE_THYSTAME] = 0.05f,
};

static int is_spawn_due(server_t *server)
{
    long now = get_now_ms();

    return server->last_resource_spawn_ms == 0 ||
        now - server->last_resource_spawn_ms >= 20000 / server->args->freq;
}

static void spawn_resource_randomly(server_t *server, int resource)
{
    int x = rand() % server->args->width;
    int y = rand() % server->args->height;
    tile_t *tile = get_tile(server, x, y);

    if (tile)
        tile->resources[resource]++;
}

static int count_row_resource(server_t *server, int y, int resource)
{
    int total = 0;
    tile_t *tile;

    for (int x = 0; x < server->args->width; x++) {
        tile = get_tile(server, x, y);
        if (tile)
            total += tile->resources[resource];
    }
    return total;
}

static int count_current_resource(server_t *server, int resource)
{
    int total = 0;

    for (int y = 0; y < server->args->height; y++)
        total += count_row_resource(server, y, resource);
    return total;
}

int respawn_world_resources(server_t *server)
{
    int map_size;
    int target;
    int current;
    int missing;

    if (!server || !is_spawn_due(server))
        return 0;
    map_size = server->args->width * server->args->height;
    for (int res = 0; res < RESOURCE_COUNT; res++) {
        target = (int)(map_size * resource_density[res]);
        if (target < 1)
            target = 1;
        current = count_current_resource(server, res);
        missing = target - current;
        for (int i = 0; i < missing; i++)
            spawn_resource_randomly(server, res);
    }
    server->last_resource_spawn_ms = get_now_ms();
    return 1;
}
