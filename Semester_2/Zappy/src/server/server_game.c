/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_game
*/

#include "../../include/server/server_internal.h"

static int is_spawn_due(t_server *server)
{
    long now = get_now_ms();
    return server->last_resource_spawn_ms == 0 ||
        now - server->last_resource_spawn_ms >= 20000 / server->args->freq;
}

int respawn_world_resources(t_server *server)
{
    if (!server || !server->world || !is_spawn_due(server))
        return 0;
    top_up_world_resources(server);
    server->last_resource_spawn_ms = get_now_ms();
    return 1;
}
