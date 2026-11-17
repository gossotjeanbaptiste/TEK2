/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_command_fork - fork command implementation (egg creation)
*/

#include "../../include/server/server_internal.h"

static void fill_egg(t_server *server, t_player *player, t_egg *egg)
{
    egg->id = server->next_egg_id++;
    egg->x = player->x;
    egg->y = player->y;
    egg->team_index = player->team_index;
}

static int add_egg(t_server *server, t_player *player)
{
    if (!server || !player || server->nb_eggs >= MAX_CLIENTS)
        return 0;
    fill_egg(server, player, &server->eggs[server->nb_eggs]);
    server->nb_eggs++;
    return 1;
}

int run_fork(t_server *server, t_client *client, t_player *player)
{
    t_egg *egg;

    if (!server || !player || !add_egg(server, player))
        return enqueue_write(client, "ko\n", 3);
    egg = &server->eggs[server->nb_eggs - 1];
    notify_gui(server, "pfk %d\n", player->id);
    notify_gui(server, "enw %d %d %d %d\n", egg->id, player->id,
        egg->x, egg->y);
    return finish_ok_command(server, client, player);
}
