/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_command_resource
*/

#include "../../include/server/server_internal.h"

int run_take(t_server *server, t_client *client, t_player *player, int res)
{
    t_tile *tile = get_tile(server, player->x, player->y);

    if (!tile || tile->resources[res] <= 0)
        return finish_ko_command(server, client, player);
    tile->resources[res]--;
    player->inventory[res]++;
    notify_gui(server, "pgt %d %d\n", player->id, res);
    notify_gui_inventory(server, player);
    notify_gui_tile(server, player->x, player->y);
    return finish_ok_command(server, client, player);
}

int run_set(t_server *server, t_client *client, t_player *player, int res)
{
    t_tile *tile = get_tile(server, player->x, player->y);

    if (!tile || player->inventory[res] <= 0)
        return finish_ko_command(server, client, player);
    player->inventory[res]--;
    tile->resources[res]++;
    notify_gui(server, "pdr %d %d\n", player->id, res);
    notify_gui_inventory(server, player);
    notify_gui_tile(server, player->x, player->y);
    return finish_ok_command(server, client, player);
}
