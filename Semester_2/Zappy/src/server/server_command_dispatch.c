/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_command_dispatch
*/

#include <string.h>
#include <stdio.h>
#include "../../include/server/server_internal.h"

static int dispatch_simple(server_t *server, client_t *client,
    player_t *player, const char *cmd)
{
    if (strcmp(cmd, "Forward") == 0)
        return run_forward(server, client, player);
    if (strcmp(cmd, "Right") == 0)
        return run_right(server, client, player);
    if (strcmp(cmd, "Left") == 0)
        return run_left(server, client, player);
    if (strcmp(cmd, "Inventory") == 0)
        return run_inventory(server, client, player);
    if (strcmp(cmd, "Look") == 0)
        return run_look(server, client, player);
    if (strcmp(cmd, "Eject") == 0)
        return run_eject(server, client, player);
    if (strcmp(cmd, "Fork") == 0)
        return run_fork(server, client, player);
    if (strcmp(cmd, "Incantation") == 0)
        return run_incantation(server, client, player);
    return -1;
}

static int dispatch_with_arg(server_t *server, client_t *client,
    player_t *player, const char *cmd)
{
    char verb[64];
    char arg[BUFFER_SIZE];
    int count = sscanf(cmd, "%63s", verb);
    const char *msg_start = strchr(cmd, ' ');
    int res = -1;

    if (count == 1 && strcmp(verb, "Connect_nbr") == 0)
        return run_connect_nbr(server, client, player);
    if (count >= 1 && strcmp(verb, "Broadcast") == 0 && msg_start)
        return run_broadcast(server, client, player, msg_start + 1);
    count = sscanf(cmd, "%63s %63s", verb, arg);
    res = (count == 2) ? resource_index(arg) : -1;
    if (count == 2 && strcmp(verb, "Take") == 0 && res >= 0)
        return run_take(server, client, player, res);
    if (count == 2 && strcmp(verb, "Set") == 0 && res >= 0)
        return run_set(server, client, player, res);
    return -1;
}

int execute_player_command(server_t *server, player_t *player)
{
    const char *cmd = player->commands[0];
    client_t *client = get_client_by_fd(server, player->client_fd);
    int result;

    if (player->command_count == 0)
        return 1;
    result = dispatch_simple(server, client, player, cmd);
    if (result != -1)
        return result;
    result = dispatch_with_arg(server, client, player, cmd);
    if (result != -1)
        return result;
    return finish_ko_command(server, client, player);
}
