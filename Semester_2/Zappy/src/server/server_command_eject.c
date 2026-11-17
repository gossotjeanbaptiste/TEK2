/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_command_eject - eject command implementation
*/

#include "../../include/server/server_internal.h"

static void eject_player_in_direction(t_server *server, t_player *ejector,
    t_player *target)
{
    if (ejector->dir == DIR_NORTH)
        target->y = wrap_coord(target->y - 1, server->args->height);
    if (ejector->dir == DIR_SOUTH)
        target->y = wrap_coord(target->y + 1, server->args->height);
    if (ejector->dir == DIR_WEST)
        target->x = wrap_coord(target->x - 1, server->args->width);
    if (ejector->dir == DIR_EAST)
        target->x = wrap_coord(target->x + 1, server->args->width);
    notify_gui_position(server, target);
}

int run_eject(t_server *server, t_client *client, t_player *player)
{
    notify_gui(server, "pex %d\n", player->id);
    for (int i = 0; i < server->nb_players; i++) {
        t_player *other = &server->players[i];
        if (other->client_fd != player->client_fd &&
            other->x == player->x && other->y == player->y) {
            eject_player_in_direction(server, player, other);
        }
    }
    return finish_ok_command(server, client, player);
}
