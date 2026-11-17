/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_command_incantation
*/

#include <string.h>
#include "../../include/server/server_internal.h"

static const int required_players_by_level[8] = {
    0, 1, 2, 2, 4, 4, 6, 6
};

static const int required_resources_by_level[8][RESOURCE_COUNT] = {
    {0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0},
    {0, 2, 0, 1, 0, 2, 0},
    {0, 1, 1, 2, 0, 1, 0},
    {0, 1, 2, 1, 3, 0, 0},
    {0, 1, 2, 3, 0, 1, 0},
    {0, 2, 2, 2, 2, 2, 1},
};

static int count_tile_players(t_server *server, t_player *player)
{
    int count = 0;

    for (int i = 0; i < server->nb_players; i++) {
        t_player *other = &server->players[i];
        if (other->team_index != player->team_index)
            continue;
        if (other->level != player->level)
            continue;
        if (other->x != player->x || other->y != player->y)
            continue;
        if (other->incanting)
            continue;
        count++;
    }
    return count;
}

static int tile_has_resources(t_tile *tile, int level)
{
    if (!tile || level < 1 || level > 7)
        return 0;

    for (int res = 0; res < RESOURCE_COUNT; res++) {
        if (tile->resources[res] < required_resources_by_level[level][res])
            return 0;
    }
    return 1;
}

static void remove_incantation_resources(t_tile *tile, int level)
{
    for (int res = 0; res < RESOURCE_COUNT; res++) {
        tile->resources[res] -= required_resources_by_level[level][res];
        if (tile->resources[res] < 0)
            tile->resources[res] = 0;
    }
}

static void freeze_incantation_players(t_server *server, t_player *player,
    long now)
{
    for (int i = 0; i < server->nb_players; i++) {
        t_player *other = &server->players[i];

        if (other->team_index != player->team_index)
            continue;
        if (other->level != player->level)
            continue;
        if (other->x != player->x || other->y != player->y)
            continue;
        other->incanting = 1;
        other->incant_end_ms = now + 300 * 1000 / server->args->freq;
        other->next_action_time_ms = other->incant_end_ms;
    }
}

int run_incantation(t_server *server, t_client *client, t_player *player)
{
    t_tile *tile;

    if (!server || !client || !player || !server->args || !server->args->freq)
        return 0;
    if (player->incanting)
        return enqueue_write(client, "ko\n", 3);
    tile = get_tile(server, player->x, player->y);
    if (!tile || player->level < 1 || player->level > 7)
        return finish_ko_command(server, client, player);
    if (count_tile_players(server, player)
        < required_players_by_level[player->level]
        || !tile_has_resources(tile, player->level))
        return finish_ko_command(server, client, player);
    freeze_incantation_players(server, player, get_now_ms());
    notify_gui_incantation_start(server, player);
    return 1;
}

int finish_incantation(t_server *server, t_client *client, t_player *player)
{
    int level;
    int participants = 0;
    t_player *candidates[MAX_CLIENTS];
    t_tile *tile;
    char response[BUFFER_SIZE];
    int len;

    if (!server || !client || !player)
        return 0;
    level = player->level;
    tile = get_tile(server, player->x, player->y);
    if (!tile)
        return enqueue_write(client, "ko\n", 3);
    for (int i = 0; i < server->nb_players; i++) {
        t_player *other = &server->players[i];

        if (!other->incanting)
            continue;
        if (other->team_index != player->team_index)
            continue;
        if (other->level != level)
            continue;
        if (other->x != player->x || other->y != player->y)
            continue;
        candidates[participants++] = other;
    }
    if (participants < required_players_by_level[level]
        || !tile_has_resources(tile, level)) {
        notify_gui(server, "pie %d %d 0\n", player->x, player->y);
        for (int i = 0; i < participants; i++) {
            t_player *other = candidates[i];
            t_client *c = get_client_by_fd(server, other->client_fd);

            other->incanting = 0;
            if (c)
                enqueue_write(c, "ko\n", 3);
            if (other->command_count > 0 &&
                strcmp(other->commands[0], "Incantation") == 0)
                pop_player_command(server, other);
        }
        return 1;
    }
    remove_incantation_resources(tile, level);
    notify_gui(server, "pie %d %d 1\n", player->x, player->y);
    notify_gui_tile(server, player->x, player->y);
    for (int i = 0; i < participants; i++) {
        t_player *other = candidates[i];
        t_client *c = get_client_by_fd(server, other->client_fd);

        other->incanting = 0;
        other->level++;
        notify_gui(server, "plv %d %d\n", other->id, other->level);
        if (c) {
            len = snprintf(response, sizeof(response),
                "Current level: %d\n", other->level);
            if (len < 0 || !enqueue_write(c, response, len))
                return 0;
        }
        if (other->command_count > 0 &&
            strcmp(other->commands[0], "Incantation") == 0)
            pop_player_command(server, other);
    }
    return check_victory(server);
}
