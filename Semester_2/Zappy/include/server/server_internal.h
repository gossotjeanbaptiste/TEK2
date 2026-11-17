/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_internal
*/

#ifndef SERVER_INTERNAL_H
    #define SERVER_INTERNAL_H

    #include "../../include/server/zappy_serv.h"

/* server_world.c */
t_tile  *get_tile(t_server *server, int x, int y);
int     wrap_coord(int coord, int max);
int     init_world(t_server *server);
void    top_up_world_resources(t_server *server);

/* server_players.c */
int     register_player(t_server *server, int client_index,
            const char *team_name, int team_index);
void    remove_player(t_server *server, int client_fd);
int     spawn_player_from_egg(t_server *server, int client_index,
            const char *team_name, int team_index);
t_player    *find_player_by_fd(t_server *server, int client_fd);
long    get_now_ms(void);

/* server_notify.c */
int     notify_gui_position(t_server *server, t_player *player);
int     notify_gui_new_player(t_server *server, t_player *player);

/* server_notify_events.c */
void    notify_gui(t_server *server, const char *fmt, ...);
void    notify_gui_tile(t_server *server, int x, int y);
void    notify_gui_inventory(t_server *server, t_player *player);
void    notify_gui_incantation_start(t_server *server, t_player *player);

/* server_buffer.c */
int     enqueue_write(t_client *client, const char *data, int len);
int     enqueue_gui_dump(t_server *server, t_client *client);

/* server_resource.c */
int     resource_index(const char *name);

/* server_commands.c */
int     queue_player_command(t_player *player, const char *cmd);
long    command_duration_ms(t_server *server, const char *cmd);
int     process_player_command(t_server *server, t_client *client,
            const char *cmd);
int     execute_player_command(t_server *server, t_player *player);
int     process_pending_actions(t_server *server);
void    pop_player_command(t_server *server, t_player *player);
int     finish_silent_command(t_server *server, t_player *player);
int     finish_ok_command(t_server *server, t_client *client,
            t_player *player);
int     finish_ko_command(t_server *server, t_client *client,
            t_player *player);
int     finish_incantation(t_server *server, t_client *client,
            t_player *player);

/* server_network.c */
t_client    *get_client_by_fd(t_server *server, int fd);
void    reset_client(t_client *client);
int     handle_client_read(t_server *server, int index);
int     handle_client_write(t_server *server, int index);

/* server_connect.c */
int     try_set_team(t_server *server, int index, const char *name);
int     accept_client(t_server *server);
void    remove_client(t_server *server, int index);

/* server_player_health.c */
int     player_lose_food(t_server *server, t_player *player, int amount);
int     is_player_dead(t_player *player);
void    process_food_decay(t_server *server);

/* server_command_broadcast.c */
int     run_broadcast(t_server *server, t_client *client, t_player *player,
            const char *message);
int     get_player_x(t_server *server, int client_fd);
int     get_player_y(t_server *server, int client_fd);

/* server_command_eject.c */
int     run_eject(t_server *server, t_client *client, t_player *player);

/* server_command_fork.c */
int     run_fork(t_server *server, t_client *client, t_player *player);

/* server_game.c */
int     respawn_world_resources(t_server *server);

/* server_game_over.c */
int     notify_gui_game_over(t_server *server, const char *team_name);
int     check_victory(t_server *server);

#endif
