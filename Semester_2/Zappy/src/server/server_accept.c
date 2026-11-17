/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_accept
*/

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include "../../include/server/server_internal.h"

int accept_client(t_server *server)
{
    int client_fd = accept(server->server_fd, NULL, NULL);
    int slot = server->nb_clients;

    if (client_fd == -1)
        return 0;
    if (slot >= MAX_CLIENTS)
        return close(client_fd), 0;
    fcntl(client_fd, F_SETFL, O_NONBLOCK);
    reset_client(&server->clients[slot]);
    server->clients[slot].fd = client_fd;
    server->pollfds[slot + 1].fd = client_fd;
    server->pollfds[slot + 1].events = POLLIN | POLLOUT;
    server->pollfds[slot + 1].revents = 0;
    if (!enqueue_write(&server->clients[slot], "WELCOME\n", 8))
        return close(client_fd), 0;
    server->nb_clients++;
    return 1;
}

void remove_client(t_server *server, int index)
{
    int client_fd = server->clients[index].fd;

    close(client_fd);
    remove_player(server, client_fd);
    for (int i = index; i < server->nb_clients - 1; i++) {
        server->clients[i] = server->clients[i + 1];
        server->pollfds[i + 1] = server->pollfds[i + 2];
    }
    server->nb_clients--;
    reset_client(&server->clients[server->nb_clients]);
}
