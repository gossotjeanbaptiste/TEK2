/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_players
*/

#define _POSIX_C_SOURCE 200809L
#include <time.h>
#include <string.h>
#include "../../include/server/server_internal.h"

long get_now_ms(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000L + ts.tv_nsec / 1000000L;
}

t_player *find_player_by_fd(t_server *server, int client_fd)
{
    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].client_fd == client_fd)
            return &server->players[i];
    }
    return NULL;
}

void remove_player(t_server *server, int client_fd)
{
    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].client_fd != client_fd)
            continue;
        notify_gui(server, "pdi %d\n", server->players[i].id);
        for (int j = i; j < server->nb_players - 1; j++)
            server->players[j] = server->players[j + 1];
        server->nb_players--;
        return;
    }
}

static void fill_player_fields(t_server *server, t_player *player,
    const char *team_name, int team_index, int client_index, int x, int y)
{
    player->id = server->next_player_id++;
    player->x = x;
    player->y = y;
    player->dir = (rand() % 4) + 1;
    player->level = 1;
    player->food = 10;
    player->team_index = team_index;
    player->client_fd = server->clients[client_index].fd;
    for (int r = 0; r < RESOURCE_COUNT; r++)
        player->inventory[r] = 0;
    strncpy(player->team_name, team_name, sizeof(player->team_name) - 1);
    player->team_name[sizeof(player->team_name) - 1] = '\0';
}

static int add_player(t_server *server, t_player *player, int client_index,
    const char *team_name, int team_index, int x, int y)
{
    if (!server || !server->players || server->nb_players >= MAX_CLIENTS)
        return 0;
    fill_player_fields(server, player, team_name, team_index, client_index, x, y);
    server->nb_players++;
    notify_gui_new_player(server, player);
    return 1;
}

int register_player(t_server *server, int client_index,
    const char *team_name, int team_index)
{
    t_player *player;

    if (!server || !server->players || server->nb_players >= MAX_CLIENTS)
        return 0;
    player = &server->players[server->nb_players];
    return add_player(server, player, client_index, team_name,
        team_index, rand() % server->args->width, rand() % server->args->height);
}

int spawn_player_from_egg(t_server *server, int client_index,
    const char *team_name, int team_index)
{
    t_player *player;
    t_egg egg = {0};
    int egg_index = -1;

    if (!server || !server->players || server->nb_players >= MAX_CLIENTS)
        return 0;
    for (int i = 0; i < server->nb_eggs; i++) {
        if (server->eggs[i].team_index != team_index)
            continue;
        egg_index = i;
        break;
    }
    if (egg_index < 0)
        return register_player(server, client_index, team_name, team_index);
    egg = server->eggs[egg_index];
    notify_gui(server, "ebo %d\n", egg.id);
    player = &server->players[server->nb_players];
    for (int i = egg_index; i + 1 < server->nb_eggs; i++)
        server->eggs[i] = server->eggs[i + 1];
    server->nb_eggs--;
    return add_player(server, player, client_index, team_name,
        team_index, egg.x, egg.y);
}
