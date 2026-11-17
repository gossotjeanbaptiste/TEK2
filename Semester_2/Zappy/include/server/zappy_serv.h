/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** zappy_serv
*/

#ifndef ZAPPY_H
    #define ZAPPY_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/socket.h>
    #include <sys/poll.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>

    #define MAX_TEAMS 64
    #define MAX_MAP_DIM 42
    #define DEFAULT_FREQ 100
    #define MAX_CLIENTS 1024
    #define BUFFER_SIZE 4096
    /* Le dump initial GUI (bct de toute la carte + joueurs) dépasse
    ** largement 4 Ko dès que la carte grandit : tampon d'écriture dédié. */
    #define WRITE_BUFFER_SIZE 131072

typedef enum e_resource {
    RESOURCE_FOOD,
    RESOURCE_LINEMATE,
    RESOURCE_DERAUMERE,
    RESOURCE_SIBUR,
    RESOURCE_MENDIANE,
    RESOURCE_PHIRAS,
    RESOURCE_THYSTAME,
    RESOURCE_COUNT,
} t_resource;

typedef struct s_tile {
    int resources[RESOURCE_COUNT];
} t_tile;

typedef struct s_args {
    int     port;
    int     width;
    int     height;
    int     clients_nb;
    int     freq;
    char    *teams[MAX_TEAMS];
    int     teams_count;
} t_args;

typedef enum e_client_state {
    CLIENT_STATE_HANDSHAKE,
    CLIENT_STATE_IA,
    CLIENT_STATE_GUI,
} t_client_state;

typedef enum e_direction {
    DIR_NORTH = 1,
    DIR_EAST = 2,
    DIR_SOUTH = 3,
    DIR_WEST = 4,
} t_direction;

typedef struct s_player {
    int             id;
    int             x;
    int             y;
    t_direction     dir;
    int             level;
    int             food;
    int             team_index;
    int             client_fd;
    char            team_name[64];
    int             inventory[RESOURCE_COUNT];
    char            commands[10][64];
    int             command_count;
    int             incanting;
    long            incant_end_ms;
    long            next_action_time_ms;
} t_player;

typedef struct s_egg {
    int     id;
    int     x;
    int     y;
    int     team_index;
} t_egg;

typedef struct s_client {
    int             fd;
    char            read_buffer[BUFFER_SIZE];
    char            write_buffer[WRITE_BUFFER_SIZE];
    int             read_pos;
    int             write_pos;
    t_client_state  state;
    char            team_name[64];
    int             team_index;
} t_client;

typedef struct s_server {
    int         server_fd;
    t_client    *clients;
    int         nb_clients;
    struct pollfd *pollfds;
    t_args      *args;
    t_tile      *world;
    t_player    *players;
    int         nb_players;
    int         next_player_id;
    int         nb_eggs;
    int         next_egg_id;
    t_egg       eggs[MAX_CLIENTS];
    int         game_over;
    char        winning_team[64];
    long        last_resource_spawn_ms;
    long        last_food_decay_ms;
} t_server;

int     parse_args(int argc, char **argv, t_args *args);
void    print_usage(char *binary);

int     create_server_socket(int port);
t_server *init_server(int server_fd, t_args *args);
void    server_loop(t_server *server);
void    destroy_server(t_server *server);

#endif
