/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_internal
*/

#ifndef SERVER_INTERNAL_H
    #define SERVER_INTERNAL_H
    #define _POSIX_C_SOURCE 200809L
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
    #define BUFFER_SIZE 150000

typedef enum e_resource {
    RESOURCE_FOOD,
    RESOURCE_LINEMATE,
    RESOURCE_DERAUMERE,
    RESOURCE_SIBUR,
    RESOURCE_MENDIANE,
    RESOURCE_PHIRAS,
    RESOURCE_THYSTAME,
    RESOURCE_COUNT,
} ressource_t;

typedef struct s_tile {
    int resources[RESOURCE_COUNT];
} tile_t;

typedef struct s_point {
    int x;
    int y;
} point_t;

typedef struct s_args {
    int port;
    int width;
    int height;
    int clients_nb;
    int freq;
    char *teams[MAX_TEAMS];
    int teams_count;
} args_t;

typedef struct s_argv_cursor {
    char **argv;
    int argc;
    int index;
} argv_cursor_t;

typedef enum e_client_state {
    CLIENT_STATE_HANDSHAKE,
    CLIENT_STATE_IA,
    CLIENT_STATE_GUI,
} client_state_t;

typedef enum e_direction {
    DIR_NORTH = 1,
    DIR_EAST = 2,
    DIR_SOUTH = 3,
    DIR_WEST = 4,
} direction_t;

typedef struct s_player {
    int id;
    int x;
    int y;
    direction_t dir;
    int level;
    int food;
    int team_index;
    int client_fd;
    char team_name[64];
    int inventory[RESOURCE_COUNT];
    char commands[10][64];
    int command_count;
    int incanting;
    long incant_end_ms;
    long next_action_time_ms;
    long next_food_decay_ms;
} player_t;

typedef struct s_player_spawn {
    const char *team_name;
    int team_index;
    int client_index;
    point_t position;
} player_spawn_t;

typedef struct s_egg {
    int id;
    int x;
    int y;
    int team_index;
} egg_t;

typedef struct s_look_buffer {
    char *data;
    size_t max;
    int first_token;
    int cell_index;
} look_buffer_t;

typedef struct s_incantation_session {
    player_t **players;
    int player_count;
    tile_t *tile;
    int target_level;
} incantation_session_t;

typedef struct s_client {
    int fd;
    char read_buffer[BUFFER_SIZE];
    char write_buffer[BUFFER_SIZE];
    int read_pos;
    int write_pos;
    client_state_t state;
    char team_name[64];
    int team_index;
} client_t;

typedef struct s_server {
    int server_fd;
    client_t *clients;
    int nb_clients;
    struct pollfd *pollfds;
    args_t *args;
    tile_t *world;
    player_t *players;
    int nb_players;
    int next_player_id;
    int nb_eggs;
    int next_egg_id;
    egg_t eggs[MAX_CLIENTS];
    int game_over;
    char winning_team[64];
    long last_resource_spawn_ms;
} server_t;

int parse_flag(argv_cursor_t *cursor, args_t *args, int *skip);
int parse_args(int argc, char **argv, args_t *args);
void print_usage(char *binary);
int add_team(args_t *args, char *name);
int str_to_positive_int(char *str, int *out);

int create_server_socket(int port);
server_t *init_server(int server_fd, args_t *args);
void server_loop(server_t *server);
void destroy_server(server_t *server);

int run_forward(server_t *server, client_t *client, player_t *player);
int run_right(server_t *server, client_t *client, player_t *player);
int run_left(server_t *server, client_t *client, player_t *player);
int run_inventory(server_t *server, client_t *client, player_t *player);
int run_look(server_t *server, client_t *client, player_t *player);
int run_connect_nbr(server_t *server, client_t *client, player_t *player);
int run_take(server_t *server, client_t *client, player_t *player, int res);
int run_set(server_t *server, client_t *client, player_t *player, int res);
int run_broadcast(server_t *server, client_t *client, player_t *player,
    const char *message);
int run_eject(server_t *server, client_t *client, player_t *player);
int run_fork(server_t *server, client_t *client, player_t *player);
int run_incantation(server_t *server, client_t *client, player_t *player);


/* server_world.c */
tile_t *get_tile(server_t *server, int x, int y);
int wrap_coord(int coord, int max);
int init_world(server_t *server);

/* server_players.c */
int register_player(server_t *server, int client_index,
    const char *team_name, int team_index);
void remove_player(server_t *server, int client_fd);
int spawn_player_from_egg(server_t *server, int client_index,
    const char *team_name, int team_index);
player_t *find_player_by_fd(server_t *server, int client_fd);
long get_now_ms(void);

/* server_notify.c */
int notify_gui_position(server_t *server, player_t *player);
int notify_gui_new_player(server_t *server, player_t *player);
int notify_gui_player_death(server_t *server, int player_id);

/* server_buffer.c */
int enqueue_write(client_t *client, const char *data, int len);
int enqueue_gui_dump(server_t *server, client_t *client);

/* server_resource.c */
int resource_index(const char *name);

/* server_commands.c */
int queue_player_command(player_t *player, const char *cmd);
long command_duration_ms(server_t *server, const char *cmd);
int process_player_command(server_t *server, client_t *client,
    const char *cmd);
int execute_player_command(server_t *server, player_t *player);
int process_pending_actions(server_t *server);
void pop_player_command(server_t *server, player_t *player);
int finish_silent_command(server_t *server, player_t *player);
int finish_ok_command(server_t *server, client_t *client,
    player_t *player);
int finish_ko_command(server_t *server, client_t *client,
    player_t *player);
int finish_incantation(server_t *server, client_t *client,
    player_t *player);

/* server_network.c */
client_t *get_client_by_fd(server_t *server, int fd);
void reset_client(client_t *client);
int handle_client_read(server_t *server, int index);
int handle_client_write(server_t *server, int index);

/* server_connect.c */
int try_set_team(server_t *server, int index, const char *name);
int accept_client(server_t *server);
void remove_client(server_t *server, int index);

/* server_player_health.c */
int player_lose_food(server_t *server, player_t *player, int amount);
int is_player_dead(player_t *player);
void process_food_decay(server_t *server);

/* server_command_broadcast.c */
int run_broadcast(server_t *server, client_t *client, player_t *player,
    const char *message);
int get_player_x(server_t *server, int client_fd);
int get_player_y(server_t *server, int client_fd);

/* server_command_eject.c */
int run_eject(server_t *server, client_t *client, player_t *player);

/* server_command_fork.c */
int run_fork(server_t *server, client_t *client, player_t *player);

/* server_game.c */
int respawn_world_resources(server_t *server);

/* server_game_over.c */
int notify_gui_game_over(server_t *server, const char *team_name);
int check_victory(server_t *server);

/* server_loop.c */
void compact_clients(server_t *server);

/* server_command_info.c */
int count_team_eggs(server_t *server, int team_index);

#endif
