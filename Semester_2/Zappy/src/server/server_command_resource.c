/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_command_resource
*/

#include "../../include/server/server_internal.h"

int run_take(server_t *server, client_t *client, player_t *player, int res)
{
    tile_t *tile = get_tile(server, player->x, player->y);

    if (!tile || tile->resources[res] <= 0)
        return finish_ko_command(server, client, player);
    tile->resources[res]--;
    player->inventory[res]++;
    return finish_ok_command(server, client, player);
}

int run_set(server_t *server, client_t *client, player_t *player, int res)
{
    tile_t *tile = get_tile(server, player->x, player->y);

    if (!tile || player->inventory[res] <= 0)
        return finish_ko_command(server, client, player);
    player->inventory[res]--;
    tile->resources[res]++;
    return finish_ok_command(server, client, player);
}
