/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_accept
*/

#include <unistd.h>
#include <sys/socket.h>
#include "../../include/server/server_internal.h"

int accept_client(server_t *server)
{
    int client_fd = accept(server->server_fd, NULL, NULL);

    if (client_fd == -1)
        return 0;
    if (server->nb_clients >= MAX_CLIENTS) {
        close(client_fd);
        return 0;
    }
    reset_client(&server->clients[server->nb_clients]);
    server->clients[server->nb_clients].fd = client_fd;
    if (!enqueue_write(&server->clients[server->nb_clients], "WELCOME\n", 8)) {
        close(client_fd);
        server->clients[server->nb_clients].fd = -1;
        return 0;
    }
    server->nb_clients++;
    return 1;
}

void remove_client(server_t *server, int index)
{
    if (index < 0 || index >= server->nb_clients)
        return;
    if (server->clients[index].fd != -1) {
        close(server->clients[index].fd);
        remove_player(server, server->clients[index].fd);
        server->clients[index].fd = -1;
    }
}

void compact_clients(server_t *server)
{
    int write_idx = 0;

    for (int i = 0; i < server->nb_clients; i++) {
        if (server->clients[i].fd == -1)
            continue;
        if (i != write_idx)
            server->clients[write_idx] = server->clients[i];
        write_idx++;
    }
    server->nb_clients = write_idx;
}
