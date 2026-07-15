/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_command_fork - fork command implementation (egg creation)
*/

#include "../../include/server/server_internal.h"

static void fill_egg(server_t *server, player_t *player, egg_t *egg)
{
    egg->id = server->next_egg_id;
    server->next_egg_id++;
    egg->x = player->x;
    egg->y = player->y;
    egg->team_index = player->team_index;
}

static int add_egg(server_t *server, player_t *player)
{
    if (!server || !player || server->nb_eggs >= MAX_CLIENTS)
        return 0;
    fill_egg(server, player, &server->eggs[server->nb_eggs]);
    server->nb_eggs++;
    return 1;
}

int run_fork(server_t *server, client_t *client, player_t *player)
{
    if (!server || !player)
        return enqueue_write(client, "ko\n", 3);
    if (!add_egg(server, player))
        return finish_ko_command(server, client, player);
    return finish_ok_command(server, client, player);
}
