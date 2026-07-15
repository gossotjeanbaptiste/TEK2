/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** socket
*/

#include "../../include/server/server_internal.h"

static int create_socket(void)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == -1) {
        perror("socket");
        return -1;
    }
    return sock;
}

static int set_reuse_addr(int sock)
{
    int opt = 1;

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        return 0;
    }
    return 1;
}

static int bind_socket(int sock, int port)
{
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        return 0;
    }
    return 1;
}

static int listen_socket(int sock)
{
    if (listen(sock, 128) == -1) {
        perror("listen");
        return 0;
    }
    return 1;
}

int create_server_socket(int port)
{
    int sock = create_socket();

    if (sock == -1 || !set_reuse_addr(sock) || !bind_socket(sock, port) ||
        !listen_socket(sock)) {
        if (sock != -1)
            close(sock);
        return -1;
    }
    return sock;
}
