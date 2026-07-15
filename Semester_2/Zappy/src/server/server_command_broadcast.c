/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_command_broadcast - broadcast command implementation
*/

#include <math.h>
#include "../../include/server/server_internal.h"

static int direction_from_delta(int dx, int dy)
{
    if (dy == 0 && dx > 0)
        return DIR_EAST;
    if (dy == 0 && dx < 0)
        return DIR_WEST;
    if (dx == 0 && dy < 0)
        return DIR_NORTH;
    if (dx == 0 && dy > 0)
        return DIR_SOUTH;
    if (dx > 0 && dy < 0)
        return DIR_NORTH;
    if (dx < 0 && dy < 0)
        return DIR_WEST;
    if (dx < 0 && dy > 0)
        return DIR_SOUTH;
    return DIR_EAST;
}

static int get_direction_to(const point_t *from, const point_t *to,
    int width, int height)
{
    int dx = to->x - from->x;
    int dy = to->y - from->y;

    if (dx > width / 2)
        dx -= width;
    else if (dx < -(width / 2))
        dx += width;
    if (dy > height / 2)
        dy -= height;
    else if (dy < -(height / 2))
        dy += height;
    return direction_from_delta(dx, dy);
}

static void broadcast_to_client(server_t *server, client_t *c,
    player_t *player, const char *message)
{
    point_t from = {player->x, player->y};
    point_t to;
    char response[BUFFER_SIZE];
    int dir;
    int len;

    if (c->state != CLIENT_STATE_IA)
        return;
    to.x = get_player_x(server, c->fd);
    to.y = get_player_y(server, c->fd);
    dir = get_direction_to(&from, &to, server->args->width,
        server->args->height);
    len = snprintf(response, sizeof(response), "message %d, %s\n", dir,
        message);
    enqueue_write(c, response, len);
}

int run_broadcast(server_t *server, client_t *client, player_t *player,
    const char *message)
{
    for (int i = 0; i < server->nb_clients; i++)
        broadcast_to_client(server, &server->clients[i], player, message);
    return finish_ok_command(server, client, player);
}

int get_player_x(server_t *server, int client_fd)
{
    for (int i = 0; i < server->nb_players; i++)
        if (server->players[i].client_fd == client_fd)
            return server->players[i].x;
    return 0;
}

int get_player_y(server_t *server, int client_fd)
{
    for (int i = 0; i < server->nb_players; i++)
        if (server->players[i].client_fd == client_fd)
            return server->players[i].y;
    return 0;
}
