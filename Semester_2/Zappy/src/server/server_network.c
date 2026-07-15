/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_network
*/

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "../../include/server/server_internal.h"

client_t *get_client_by_fd(server_t *server, int fd)
{
    for (int i = 0; i < server->nb_clients; i++)
        if (server->clients[i].fd == fd)
            return &server->clients[i];
    return NULL;
}

void reset_client(client_t *client)
{
    client->fd = -1;
    client->state = CLIENT_STATE_HANDSHAKE;
    client->read_pos = 0;
    client->write_pos = 0;
    client->team_name[0] = '\0';
    client->team_index = -1;
    client->read_buffer[0] = '\0';
    client->write_buffer[0] = '\0';
}

int handle_client_write(server_t *server, int index)
{
    client_t *client = &server->clients[index];
    ssize_t sent = send(client->fd, client->write_buffer,
        client->write_pos, 0);

    (void)server;
    if (sent <= 0) {
        if (sent == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
            return 1;
        return 0;
    }
    memmove(client->write_buffer, client->write_buffer + sent,
        client->write_pos - sent);
    client->write_pos -= sent;
    return 1;
}
