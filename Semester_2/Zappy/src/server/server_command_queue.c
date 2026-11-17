/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_command_queue
*/

#include <stdlib.h>
#include <string.h>
#include "../../include/server/server_internal.h"

/*
** ZAPPY_SUBJECT_TIMES=1 : durées des commandes du sujet (en unités de
** temps 1/f) au lieu d'une unité pour toutes. Utilisé par les démos lentes
** pour que les déplacements soient visibles à l'écran.
*/
static int use_subject_times(void)
{
    static int cached = -1;
    const char *env;

    if (cached < 0) {
        env = getenv("ZAPPY_SUBJECT_TIMES");
        cached = (env && env[0] && strcmp(env, "0") != 0) ? 1 : 0;
    }
    return cached;
}

static int command_units(const char *cmd)
{
    if (strncmp(cmd, "Fork", 4) == 0)
        return 42;
    if (strncmp(cmd, "Inventory", 9) == 0
        || strncmp(cmd, "Connect_nbr", 11) == 0
        || strncmp(cmd, "Incantation", 11) == 0)
        return 1;
    return 7;
}

long command_duration_ms(t_server *server, const char *cmd)
{
    int units = use_subject_times() ? command_units(cmd) : 1;

    return (long)units * 1000 / server->args->freq;
}

int queue_player_command(t_player *player, const char *cmd)
{
    int idx = player->command_count;

    if (idx >= 10)
        return 0;
    strncpy(player->commands[idx], cmd, sizeof(player->commands[0]) - 1);
    player->commands[idx][sizeof(player->commands[0]) - 1] = '\0';
    player->command_count++;
    return 1;
}

int process_player_command(t_server *server, t_client *client,
    const char *cmd)
{
    t_player *player = find_player_by_fd(server, client->fd);

    if (!player)
        return 0;
    if (player->incanting)
        return enqueue_write(client, "ko\n", 3);
    if (!queue_player_command(player, cmd))
        return enqueue_write(client, "ko\n", 3);
    if (player->command_count == 1)
        player->next_action_time_ms = get_now_ms()
            + command_duration_ms(server, cmd);
    return 1;
}
