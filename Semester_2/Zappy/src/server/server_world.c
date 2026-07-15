/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_world
*/

#include <time.h>
#include "../../include/server/server_internal.h"

int wrap_coord(int coord, int max)
{
    int wrapped;

    if (max <= 0)
        return 0;
    wrapped = coord % max;
    if (wrapped < 0)
        wrapped += max;
    return wrapped;
}

tile_t *get_tile(server_t *server, int x, int y)
{
    int wrapped_x;
    int wrapped_y;

    if (!server || !server->world)
        return NULL;
    if (server->args->width <= 0 || server->args->height <= 0)
        return NULL;
    wrapped_x = wrap_coord(x, server->args->width);
    wrapped_y = wrap_coord(y, server->args->height);
    return &server->world[wrapped_y * server->args->width + wrapped_x];
}

static void clear_tile_resources(server_t *server, int x, int y)
{
    tile_t *tile = get_tile(server, x, y);

    if (!tile)
        return;
    for (int r = 0; r < RESOURCE_COUNT; r++)
        tile->resources[r] = 0;
}

static void clear_world_resources(server_t *server)
{
    for (int y = 0; y < server->args->height; y++)
        for (int x = 0; x < server->args->width; x++)
            clear_tile_resources(server, x, y);
}

static void spread_one_resource(server_t *server, int resource)
{
    int base_count = (server->args->width * server->args->height) / 10 + 1;
    tile_t *tile;
    int x;
    int y;

    for (int i = 0; i < base_count; i++) {
        x = rand() % server->args->width;
        y = rand() % server->args->height;
        tile = get_tile(server, x, y);
        if (tile)
            tile->resources[resource]++;
    }
}

static void init_world_resources(server_t *server)
{
    clear_world_resources(server);
    for (int r = 0; r < RESOURCE_COUNT; r++)
        spread_one_resource(server, r);
}

int init_world(server_t *server)
{
    int tiles = server->args->width * server->args->height;

    if (tiles <= 0)
        return 0;
    srand((unsigned)time(NULL));
    server->world = calloc(tiles, sizeof(tile_t));
    if (!server->world)
        return 0;
    init_world_resources(server);
    return 1;
}
