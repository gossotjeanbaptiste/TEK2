/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** main_server
*/

#include <stdio.h>
#include "../../include/server/zappy_serv.h"

int main(int argc, char **argv)
{
    t_args args;
    int server_sock;
    t_server *server;

    if (argc >= 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        print_usage(argv[0]);
        return 0;
    }
    if (!parse_args(argc, argv, &args))
        return 84;
    server_sock = create_server_socket(args.port);
    if (server_sock == -1)
        return 84;
    server = init_server(server_sock, &args);
    if (!server)
        return close(server_sock), 84;
    printf("Server listening on port %d\n", args.port);
    server_loop(server);
    destroy_server(server);
    return 0;
}