/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_command_eject - eject command implementation
*/

#include "../../include/server/server_internal.h"

static void eject_player_in_direction(server_t *server, player_t *ejector,
    player_t *target)
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

static void remove_egg_at(server_t *server, int index)
{
    for (int j = index; j + 1 < server->nb_eggs; j++)
        server->eggs[j] = server->eggs[j + 1];
    server->nb_eggs--;
}

static void destroy_eggs_on_tile(server_t *server, int x, int y)
{
    for (int i = 0; i < server->nb_eggs; i++) {
        if (server->eggs[i].x != x || server->eggs[i].y != y)
            continue;
        remove_egg_at(server, i);
        i--;
    }
}

int run_eject(server_t *server, client_t *client, player_t *player)
{
    player_t *other;

    if (!server || !player)
        return 0;
    for (int i = 0; i < server->nb_players; i++) {
        other = &server->players[i];
        if (other->client_fd != player->client_fd &&
            other->x == player->x && other->y == player->y) {
            eject_player_in_direction(server, player, other);
        }
    }
    destroy_eggs_on_tile(server, player->x, player->y);
    return finish_ok_command(server, client, player);
}
