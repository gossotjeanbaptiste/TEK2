/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_command_incantation
*/

#include <string.h>
#include <stdio.h>
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

static int count_tile_players(server_t *server, player_t *player)
{
    int count = 0;
    player_t *other;

    for (int i = 0; i < server->nb_players; i++) {
        other = &server->players[i];
        if (other->team_index != player->team_index)
            continue;
        if (other->level != player->level)
            continue;
        if (other->x != player->x || other->y != player->y)
            continue;
        count++;
    }
    return count;
}

static int tile_has_resources(tile_t *tile, int level)
{
    if (!tile || level < 1 || level > 7)
        return 0;
    for (int res = 0; res < RESOURCE_COUNT; res++)
        if (tile->resources[res] < required_resources_by_level[level][res])
            return 0;
    return 1;
}

static void remove_incantation_resources(tile_t *tile, int level)
{
    for (int res = 0; res < RESOURCE_COUNT; res++) {
        tile->resources[res] -= required_resources_by_level[level][res];
        if (tile->resources[res] < 0)
            tile->resources[res] = 0;
    }
}

static void freeze_incantation_players(server_t *server,
    player_t *player, long now)
{
    player_t *other;
    client_t *c;

    for (int i = 0; i < server->nb_players; i++) {
        other = &server->players[i];
        if (other->team_index != player->team_index ||
            other->level != player->level)
            continue;
        if (other->x != player->x || other->y != player->y)
            continue;
        c = get_client_by_fd(server, other->client_fd);
        other->incanting = 1;
        other->incant_end_ms = now + 300 * 1000 / server->args->freq;
        other->next_action_time_ms = other->incant_end_ms;
        if (c)
            enqueue_write(c, "Elevation underway\n", 19);
    }
}

int run_incantation(server_t *server, client_t *client, player_t *player)
{
    tile_t *tile;

    if (!server || !client || !player || !server->args || !server->args->freq)
        return 0;
    if (player->incanting)
        return finish_ko_command(server, client, player);
    tile = get_tile(server, player->x, player->y);
    if (!tile || player->level < 1 || player->level > 7)
        return finish_ko_command(server, client, player);
    if (count_tile_players(server, player) <
        required_players_by_level[player->level]
        || !tile_has_resources(tile, player->level))
        return finish_ko_command(server, client, player);
    freeze_incantation_players(server, player, get_now_ms());
    return 1;
}

static void handle_failed_incantation(server_t *server,
    player_t **candidates, int count)
{
    player_t *other;
    client_t *c;

    for (int i = 0; i < count; i++) {
        other = candidates[i];
        c = get_client_by_fd(server, other->client_fd);
        other->incanting = 0;
        if (c)
            enqueue_write(c, "ko\n", 3);
        if (other->command_count > 0 &&
            strcmp(other->commands[0], "Incantation") == 0)
            pop_player_command(server, other);
    }
}

static int notify_level_up(client_t *c, player_t *other)
{
    char response[BUFFER_SIZE];
    int len;

    if (!c)
        return 1;
    len = snprintf(response, sizeof(response),
        "Current level: %d\n", other->level);
    if (len < 0)
        return 0;
    return enqueue_write(c, response, len);
}

static int handle_successful_incantation(server_t *server,
    incantation_session_t *session)
{
    player_t *other;
    client_t *c;

    remove_incantation_resources(session->tile, session->target_level);
    for (int i = 0; i < session->player_count; i++) {
        other = session->players[i];
        c = get_client_by_fd(server, other->client_fd);
        other->incanting = 0;
        other->level++;
        if (!notify_level_up(c, other))
            return 0;
        if (other->command_count > 0 &&
            strcmp(other->commands[0], "Incantation") == 0)
            pop_player_command(server, other);
    }
    return check_victory(server);
}

static int collect_candidates(server_t *server, player_t *player,
    player_t **candidates)
{
    int participants = 0;
    player_t *other;

    for (int i = 0; i < server->nb_players; i++) {
        other = &server->players[i];
        if (!other->incanting || other->team_index != player->team_index)
            continue;
        if (other->level != player->level || other->x != player->x ||
            other->y != player->y)
            continue;
        candidates[participants] = other;
        participants++;
    }
    return participants;
}

int finish_incantation(server_t *server, client_t *client, player_t *player)
{
    player_t *candidates[MAX_CLIENTS];
    tile_t *tile = get_tile(server, player->x, player->y);
    incantation_session_t session;

    if (!server || !client || !player || !tile)
        return 0;
    session.player_count = collect_candidates(server, player, candidates);
    session.players = candidates;
    session.tile = tile;
    session.target_level = player->level;
    if (session.player_count < required_players_by_level[player->level] ||
        !tile_has_resources(tile, player->level)) {
        handle_failed_incantation(server, candidates, session.player_count);
        return 1;
    }
    return handle_successful_incantation(server, &session);
}
