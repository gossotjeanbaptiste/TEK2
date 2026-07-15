/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_command_info
*/

#include <string.h>
#include <stdio.h>
#include "../../include/server/server_internal.h"

static const char *const res_names[RESOURCE_COUNT] = {
    "food", "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"
};

int run_inventory(server_t *server, client_t *client, player_t *player)
{
    char res[BUFFER_SIZE];
    int len = snprintf(res, sizeof(res),
        "[food %d, linemate %d, deraumere %d, sibur %d, "
        "mendiane %d, phiras %d, thystame %d]\n",
        player->food, player->inventory[RESOURCE_LINEMATE],
        player->inventory[RESOURCE_DERAUMERE],
        player->inventory[RESOURCE_SIBUR],
        player->inventory[RESOURCE_MENDIANE],
        player->inventory[RESOURCE_PHIRAS],
        player->inventory[RESOURCE_THYSTAME]);

    if (len < 0 || !enqueue_write(client, res, len))
        return 0;
    return finish_silent_command(server, player);
}

static void get_look_offset(int dir, int depth, int width, point_t *offset)
{
    if (dir == DIR_NORTH) {
        offset->x = width;
        offset->y = -depth;
    }
    if (dir == DIR_SOUTH) {
        offset->x = -width;
        offset->y = depth;
    }
    if (dir == DIR_WEST) {
        offset->x = -depth;
        offset->y = -width;
    }
    if (dir == DIR_EAST) {
        offset->x = depth;
        offset->y = width;
    }
}

static void append_token(look_buffer_t *out, const char *text)
{
    if (!out->first_token)
        strncat(out->data, " ", out->max - strlen(out->data) - 1);
    strncat(out->data, text, out->max - strlen(out->data) - 1);
    out->first_token = 0;
}

static void append_tile_content(server_t *server, tile_t *tile,
    look_buffer_t *out)
{
    tile_t *player_tile;

    for (int i = 0; i < server->nb_players; i++) {
        player_tile = get_tile(server, server->players[i].x,
            server->players[i].y);
        if (player_tile != tile)
            continue;
        append_token(out, "player");
    }
    for (int r = 0; r < RESOURCE_COUNT; r++) {
        for (int q = 0; q < tile->resources[r]; q++)
            append_token(out, res_names[r]);
    }
}

static void append_look_cell(server_t *server, player_t *player,
    look_buffer_t *out, point_t *offset)
{
    tile_t *tile = get_tile(server, player->x + offset->x,
        player->y + offset->y);

    if (out->cell_index > 0)
        strncat(out->data, ",", out->max - strlen(out->data) - 1);
    out->first_token = 1;
    if (tile)
        append_tile_content(server, tile, out);
    out->cell_index++;
}

static void build_look_string(server_t *server, player_t *player,
    look_buffer_t *out)
{
    point_t offset;

    for (int depth = 0; depth <= player->level; depth++) {
        for (int width = -depth; width <= depth; width++) {
            get_look_offset(player->dir, depth, width, &offset);
            append_look_cell(server, player, out, &offset);
        }
    }
}

int run_look(server_t *server, client_t *client, player_t *player)
{
    char response[BUFFER_SIZE] = "[";
    look_buffer_t out = {response, sizeof(response), 1, 0};

    build_look_string(server, player, &out);
    strncat(response, "]\n", sizeof(response) - strlen(response) - 1);
    if (!enqueue_write(client, response, strlen(response)))
        return 0;
    return finish_silent_command(server, player);
}

int count_team_eggs(server_t *server, int team_index)
{
    int count = 0;

    for (int i = 0; i < server->nb_eggs; i++) {
        if (server->eggs[i].team_index == team_index)
            count++;
    }
    return count;
}

int run_connect_nbr(server_t *server, client_t *client, player_t *player)
{
    char response[BUFFER_SIZE];
    int remaining = count_team_eggs(server, player->team_index);
    int len;

    len = snprintf(response, sizeof(response), "%d\n", remaining);
    if (len < 0 || !enqueue_write(client, response, len))
        return 0;
    return finish_silent_command(server, player);
}
