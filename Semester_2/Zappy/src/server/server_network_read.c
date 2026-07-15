/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_network_read
*/

#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include "../../include/server/server_internal.h"

static int dispatch_line(server_t *server, client_t *client, int index)
{
    if (client->state == CLIENT_STATE_HANDSHAKE)
        return try_set_team(server, index, client->read_buffer);
    return process_player_command(server, client, client->read_buffer);
}

static int consume_lines(server_t *server, client_t *client, int index)
{
    int remain;

    for (int i = 0; i < client->read_pos; i++) {
        if (client->read_buffer[i] != '\n')
            continue;
        client->read_buffer[i] = '\0';
        if (!dispatch_line(server, client, index))
            return 0;
        remain = client->read_pos - (i + 1);
        memmove(client->read_buffer, client->read_buffer + i + 1, remain);
        client->read_pos = remain;
        i = -1;
    }
    return 1;
}

int handle_client_read(server_t *server, int index)
{
    client_t *client = &server->clients[index];
    ssize_t received = recv(client->fd,
        client->read_buffer + client->read_pos,
        BUFFER_SIZE - client->read_pos - 1, 0);

    if (received <= 0) {
        if (received == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
            return 1;
        return 0;
    }
    client->read_pos += received;
    client->read_buffer[client->read_pos] = '\0';
    return consume_lines(server, client, index);
}
