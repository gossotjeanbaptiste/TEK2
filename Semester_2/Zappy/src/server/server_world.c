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

t_tile *get_tile(t_server *server, int x, int y)
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

static const double RESOURCE_DENSITY[RESOURCE_COUNT] = {
    0.5, 0.3, 0.15, 0.1, 0.1, 0.08, 0.05
};

static int resource_target(t_server *server, int resource)
{
    int tiles = server->args->width * server->args->height;
    int target = (int)(tiles * RESOURCE_DENSITY[resource]);

    return target < 1 ? 1 : target;
}

static int count_resource_on_map(t_server *server, int resource)
{
    int tiles = server->args->width * server->args->height;
    int total = 0;

    for (int i = 0; i < tiles; i++)
        total += server->world[i].resources[resource];
    return total;
}

void top_up_world_resources(t_server *server)
{
    int tiles = server->args->width * server->args->height;
    int missing;

    for (int r = 0; r < RESOURCE_COUNT; r++) {
        missing = resource_target(server, r) - count_resource_on_map(server, r);
        for (int i = 0; i < missing; i++) {
            int t = rand() % tiles;

            server->world[t].resources[r]++;
            notify_gui_tile(server, t % server->args->width,
                t / server->args->width);
        }
    }
}

int init_world(t_server *server)
{
    int tiles = server->args->width * server->args->height;

    if (tiles <= 0)
        return 0;
    srand((unsigned)time(NULL));
    server->world = calloc(tiles, sizeof(t_tile));
    if (!server->world)
        return 0;
    top_up_world_resources(server);
    return 1;
}
