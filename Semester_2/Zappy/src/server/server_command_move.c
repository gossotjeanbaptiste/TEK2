/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_command_move
*/

#include <string.h>
#include "../../include/server/server_internal.h"

static void apply_forward(server_t *server, player_t *player)
{
    if (player->dir == DIR_NORTH)
        player->y = wrap_coord(player->y - 1, server->args->height);
    if (player->dir == DIR_SOUTH)
        player->y = wrap_coord(player->y + 1, server->args->height);
    if (player->dir == DIR_WEST)
        player->x = wrap_coord(player->x - 1, server->args->width);
    if (player->dir == DIR_EAST)
        player->x = wrap_coord(player->x + 1, server->args->width);
}

int run_forward(server_t *server, client_t *client, player_t *player)
{
    apply_forward(server, player);
    return finish_ok_command(server, client, player);
}

int run_right(server_t *server, client_t *client, player_t *player)
{
    player->dir = (player->dir % 4) + 1;
    return finish_ok_command(server, client, player);
}

int run_left(server_t *server, client_t *client, player_t *player)
{
    player->dir = (player->dir == DIR_NORTH) ? DIR_WEST : player->dir - 1;
    return finish_ok_command(server, client, player);
}
