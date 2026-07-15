/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server utils
*/

#include <unistd.h>
#include "../../include/server/server_internal.h"

void destroy_server(server_t *server)
{
    if (!server)
        return;
    for (int i = 0; i < server->nb_clients; i++)
        close(server->clients[i].fd);
    close(server->server_fd);
    free(server->clients);
    free(server->pollfds);
    free(server->players);
    free(server->world);
    free(server);
}
