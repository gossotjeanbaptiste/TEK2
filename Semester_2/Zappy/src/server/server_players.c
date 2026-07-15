/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_players
*/

#include "../../include/server/server_internal.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>

long get_now_ms(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000L + ts.tv_nsec / 1000000L;
}

player_t *find_player_by_fd(server_t *server, int client_fd)
{
    for (int i = 0; i < server->nb_players; i++)
        if (server->players[i].client_fd == client_fd)
            return &server->players[i];
    return NULL;
}

void remove_player(server_t *server, int client_fd)
{
    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].client_fd != client_fd)
            continue;
        notify_gui_player_death(server, server->players[i].id);
        for (int j = i; j < server->nb_players - 1; j++)
            server->players[j] = server->players[j + 1];
        server->nb_players--;
        return;
    }
}

static long compute_food_decay_interval(server_t *server)
{
    long interval = (126L * 1000L) / server->args->freq;

    return (interval < 1) ? 1 : interval;
}

static void fill_player_fields(server_t *server, player_t *player,
    const player_spawn_t *spawn)
{
    long now = get_now_ms();

    player->id = server->next_player_id;
    server->next_player_id++;
    player->x = spawn->position.x;
    player->y = spawn->position.y;
    player->dir = (rand() % 4) + 1;
    player->level = 1;
    player->food = 10;
    player->team_index = spawn->team_index;
    player->client_fd = server->clients[spawn->client_index].fd;
    for (int r = 0; r < RESOURCE_COUNT; r++)
        player->inventory[r] = 0;
    strncpy(player->team_name, spawn->team_name,
        sizeof(player->team_name) - 1);
    player->team_name[sizeof(player->team_name) - 1] = '\0';
    player->next_food_decay_ms = now + compute_food_decay_interval(server);
}

static int add_player(server_t *server, player_t *player,
    const player_spawn_t *spawn)
{
    if (!server || !server->players || server->nb_players >= MAX_CLIENTS)
        return 0;
    fill_player_fields(server, player, spawn);
    server->nb_players++;
    notify_gui_new_player(server, player);
    return 1;
}

static int find_team_egg_index(server_t *server, int team_index)
{
    for (int i = 0; i < server->nb_eggs; i++)
        if (server->eggs[i].team_index == team_index)
            return i;
    return -1;
}

static void remove_egg_at(server_t *server, int index)
{
    for (int i = index; i + 1 < server->nb_eggs; i++)
        server->eggs[i] = server->eggs[i + 1];
    server->nb_eggs--;
}

int spawn_player_from_egg(server_t *server, int client_index,
    const char *team_name, int team_index)
{
    player_spawn_t spawn;
    egg_t egg;
    int egg_index;

    if (!server || !server->players || server->nb_players >= MAX_CLIENTS)
        return 0;
    egg_index = find_team_egg_index(server, team_index);
    if (egg_index < 0)
        return 0;
    egg = server->eggs[egg_index];
    remove_egg_at(server, egg_index);
    spawn.team_name = team_name;
    spawn.team_index = team_index;
    spawn.client_index = client_index;
    spawn.position.x = egg.x;
    spawn.position.y = egg.y;
    return add_player(server, &server->players[server->nb_players], &spawn);
}
