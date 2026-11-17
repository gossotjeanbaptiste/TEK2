/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_command_broadcast - broadcast command implementation
*/

#include <math.h>
#include "../../include/server/server_internal.h"

static int get_direction_to(int from_x, int from_y, int to_x, int to_y,
    int width, int height)
{
    int dx = to_x - from_x;
    int dy = to_y - from_y;

    if (dx > width / 2)
        dx -= width;
    else if (dx < -(width / 2))
        dx += width;
    if (dy > height / 2)
        dy -= height;
    else if (dy < -(height / 2))
        dy += height;
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

int run_broadcast(t_server *server, t_client *client, t_player *player,
    const char *message)
{
    char response[BUFFER_SIZE];
    int len;

    notify_gui(server, "pbc %d %s\n", player->id, message);
    for (int i = 0; i < server->nb_clients; i++) {
        t_client *c = &server->clients[i];
        if (c->state == CLIENT_STATE_IA) {
            int dir = get_direction_to(player->x, player->y,
                get_player_x(server, c->fd),
                get_player_y(server, c->fd),
                server->args->width, server->args->height);
            len = snprintf(response, sizeof(response),
                "message %d, %s\n", dir, message);
            enqueue_write(c, response, len);
        }
    }
    return finish_ok_command(server, client, player);
}

int get_player_x(t_server *server, int client_fd)
{
    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].client_fd == client_fd)
            return server->players[i].x;
    }
    return 0;
}

int get_player_y(t_server *server, int client_fd)
{
    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].client_fd == client_fd)
            return server->players[i].y;
    }
    return 0;
}
