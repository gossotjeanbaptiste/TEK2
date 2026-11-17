/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_command_info
*/

#include "../../include/server/server_internal.h"

int run_inventory(t_server *server, t_client *client, t_player *player)
{
    char response[BUFFER_SIZE];
    int len = snprintf(response, sizeof(response),
        "food %d linemate %d deraumere %d sibur %d"
        " mendiane %d phiras %d thystame %d\n",
        player->inventory[RESOURCE_FOOD],
        player->inventory[RESOURCE_LINEMATE],
        player->inventory[RESOURCE_DERAUMERE],
        player->inventory[RESOURCE_SIBUR],
        player->inventory[RESOURCE_MENDIANE],
        player->inventory[RESOURCE_PHIRAS],
        player->inventory[RESOURCE_THYSTAME]);

    if (len < 0 || !enqueue_write(client, response, len))
        return 0;
    return finish_silent_command(server, player);
}

int run_look(t_server *server, t_client *client, t_player *player)
{
    t_tile *tile = get_tile(server, player->x, player->y);
    char response[BUFFER_SIZE];
    int len;

    if (!tile)
        return enqueue_write(client, "ko\n", 3);
    len = snprintf(response, sizeof(response),
        "[food %d, linemate %d, deraumere %d, sibur %d,"
        " mendiane %d, phiras %d, thystame %d]\n",
        tile->resources[RESOURCE_FOOD],
        tile->resources[RESOURCE_LINEMATE],
        tile->resources[RESOURCE_DERAUMERE],
        tile->resources[RESOURCE_SIBUR],
        tile->resources[RESOURCE_MENDIANE],
        tile->resources[RESOURCE_PHIRAS],
        tile->resources[RESOURCE_THYSTAME]);
    if (len < 0 || !enqueue_write(client, response, len))
        return 0;
    return finish_silent_command(server, player);
}

static int count_team_slots_used(t_server *server, int team_index)
{
    int count = 0;

    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].team_index == team_index)
            count++;
    }
    for (int i = 0; i < server->nb_eggs; i++) {
        if (server->eggs[i].team_index == team_index)
            count++;
    }
    return count;
}

int run_connect_nbr(t_server *server, t_client *client, t_player *player)
{
    int used = count_team_slots_used(server, player->team_index);
    int remaining = server->args->clients_nb - used;
    char response[BUFFER_SIZE];
    int len = snprintf(response, sizeof(response), "%d\n", remaining);

    if (len < 0 || !enqueue_write(client, response, len))
        return 0;
    return finish_silent_command(server, player);
}
