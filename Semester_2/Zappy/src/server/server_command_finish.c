/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_command_finish
*/

#include <string.h>
#include "../../include/server/server_internal.h"

void pop_player_command(server_t *server, player_t *player)
{
    long delay;

    memmove(player->commands[0], player->commands[1],
        (player->command_count - 1) * sizeof(player->commands[0]));
    player->command_count--;
    if (player->command_count <= 0) {
        player->next_action_time_ms = 0;
        return;
    }
    delay = command_duration_ms(server, player->commands[0]);
    player->next_action_time_ms = get_now_ms() + delay;
}

int finish_silent_command(server_t *server, player_t *player)
{
    pop_player_command(server, player);
    return 1;
}

int finish_ok_command(server_t *server, client_t *client, player_t *player)
{
    pop_player_command(server, player);
    if (!enqueue_write(client, "ok\n", 3))
        return 0;
    if (!notify_gui_position(server, player))
        return 0;
    return check_victory(server);
}

int finish_ko_command(server_t *server, client_t *client, player_t *player)
{
    pop_player_command(server, player);
    return enqueue_write(client, "ko\n", 3);
}
