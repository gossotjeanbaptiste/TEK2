/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** main_server
*/

#include <stdio.h>
#include "../../include/server/server_internal.h"

static int is_help_flag(int argc, char **argv)
{
    return argc >= 2 && (strcmp(argv[1], "--help") == 0 ||
        strcmp(argv[1], "-h") == 0);
}

static server_t *setup_server(int argc, char **argv, args_t *args)
{
    int server_sock;
    server_t *server;

    if (!parse_args(argc, argv, args))
        return NULL;
    server_sock = create_server_socket(args->port);
    if (server_sock == -1)
        return NULL;
    server = init_server(server_sock, args);
    if (!server) {
        close(server_sock);
        return NULL;
    }
    printf("Server listening on port %d\n", args->port);
    return server;
}

int main(int argc, char **argv)
{
    args_t args;
    server_t *server;

    if (is_help_flag(argc, argv)) {
        print_usage(argv[0]);
        return 0;
    }
    server = setup_server(argc, argv, &args);
    if (!server)
        return 84;
    server_loop(server);
    destroy_server(server);
    return 0;
}
