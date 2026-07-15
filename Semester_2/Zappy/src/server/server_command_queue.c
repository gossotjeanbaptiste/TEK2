/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_command_queue
*/

#include <string.h>
#include "../../include/server/server_internal.h"

static long time_unit_to_ms(server_t *server, long unit)
{
    long duration;

    if (!server || !server->args || server->args->freq <= 0)
        return 1000;
    duration = (unit * 1000L) / server->args->freq;
    return (duration < 1) ? 1 : duration;
}

long command_duration_ms(server_t *server, const char *cmd)
{
    if (!cmd)
        return time_unit_to_ms(server, 1);
    if (strncmp(cmd, "Connect_nbr", 11) == 0)
        return 0;
    if (strcmp(cmd, "Inventory") == 0)
        return time_unit_to_ms(server, 1);
    if (strcmp(cmd, "Fork") == 0)
        return time_unit_to_ms(server, 42);
    if (strcmp(cmd, "Incantation") == 0)
        return time_unit_to_ms(server, 300);
    return time_unit_to_ms(server, 7);
}

int queue_player_command(player_t *player, const char *cmd)
{
    int idx = player->command_count;

    if (idx >= 10)
        return 0;
    strncpy(player->commands[idx], cmd, sizeof(player->commands[0]) - 1);
    player->commands[idx][sizeof(player->commands[0]) - 1] = '\0';
    player->command_count++;
    return 1;
}

int process_player_command(server_t *server, client_t *client,
    const char *cmd)
{
    player_t *player;
    long delay;

    if (!server || !client || !cmd)
        return 0;
    player = find_player_by_fd(server, client->fd);
    if (!player)
        return 0;
    if (!queue_player_command(player, cmd))
        return enqueue_write(client, "ko\n", 3);
    if (player->command_count == 1 && !player->incanting) {
        delay = command_duration_ms(server, player->commands[0]);
        player->next_action_time_ms = get_now_ms() + delay;
    }
    return 1;
}
