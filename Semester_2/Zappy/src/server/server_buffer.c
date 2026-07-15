/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_buffer
*/

#include <string.h>
#include "../../include/server/server_internal.h"

int enqueue_write(client_t *client, const char *data, int len)
{
    int avail = BUFFER_SIZE - client->write_pos;

    if (len > avail)
        return 0;
    memcpy(client->write_buffer + client->write_pos, data, len);
    client->write_pos += len;
    return 1;
}

static int enqueue_tile_line(server_t *server, client_t *client, int x, int y)
{
    tile_t *tile = get_tile(server, x, y);
    char line[BUFFER_SIZE];
    int len;

    if (!tile)
        return 1;
    len = snprintf(line, sizeof(line),
        "bct %d %d %d %d %d %d %d %d %d\n",
        x, y,
        tile->resources[RESOURCE_FOOD],
        tile->resources[RESOURCE_LINEMATE],
        tile->resources[RESOURCE_DERAUMERE],
        tile->resources[RESOURCE_SIBUR],
        tile->resources[RESOURCE_MENDIANE],
        tile->resources[RESOURCE_PHIRAS],
        tile->resources[RESOURCE_THYSTAME]);
    if (len < 0)
        return 0;
    return enqueue_write(client, line, len);
}

static int enqueue_tile_row(server_t *server, client_t *client, int y)
{
    for (int x = 0; x < server->args->width; x++) {
        if (!enqueue_tile_line(server, client, x, y))
            return 0;
    }
    return 1;
}

static int enqueue_tile_dump(server_t *server, client_t *client)
{
    for (int y = 0; y < server->args->height; y++) {
        if (!enqueue_tile_row(server, client, y))
            return 0;
    }
    return 1;
}

static int enqueue_team_names(server_t *server, client_t *client)
{
    char line[BUFFER_SIZE];
    int len;

    len = snprintf(line, sizeof(line), "tna");
    if (len < 0 || !enqueue_write(client, line, len))
        return 0;
    for (int i = 0; i < server->args->teams_count; i++) {
        len = snprintf(line, sizeof(line), " %s", server->args->teams[i]);
        if (len < 0 || !enqueue_write(client, line, len))
            return 0;
    }
    return enqueue_write(client, "\n", 1);
}

int enqueue_gui_dump(server_t *server, client_t *client)
{
    char line[BUFFER_SIZE];
    int len = snprintf(line, sizeof(line), "msz %d %d\n",
        server->args->width, server->args->height);

    if (len < 0 || !enqueue_write(client, line, len))
        return 0;
    if (!enqueue_team_names(server, client))
        return 0;
    return enqueue_tile_dump(server, client);
}
