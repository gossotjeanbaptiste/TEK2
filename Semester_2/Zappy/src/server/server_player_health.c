/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_player_health - food decay and death management
*/

#include "../../include/server/server_internal.h"

static int consume_food_from_inventory(t_player *player)
{
    if (!player || player->inventory[RESOURCE_FOOD] <= 0)
        return 0;
    player->inventory[RESOURCE_FOOD]--;
    player->food++;
    return 1;
}

int player_lose_food(t_server *server, t_player *player, int amount)
{
    if (!server || !player)
        return 0;
    if (consume_food_from_inventory(player))
        return 1;
    player->food -= amount;
    if (player->food < 0)
        player->food = 0;
    return 1;
}

int is_player_dead(t_player *player)
{
    return (player && player->food <= 0) ? 1 : 0;
}

void notify_player_dead(t_server *server, t_client *client)
{
    if (!server || !client)
        return;
    enqueue_write(client, "dead\n", 5);
}

int remove_dead_player(t_server *server, int player_index)
{
    t_player *player = &server->players[player_index];
    t_client *client = get_client_by_fd(server, player->client_fd);

    notify_player_dead(server, client);
    notify_gui(server, "pdi %d\n", player->id);
    for (int i = player_index; i < server->nb_players - 1; i++)
        server->players[i] = server->players[i + 1];
    server->nb_players--;
    return 1;
}

void process_food_decay(t_server *server)
{
    for (int i = 0; i < server->nb_players; i++) {
        player_lose_food(server, &server->players[i], 1);
        notify_gui_inventory(server, &server->players[i]);
        if (is_player_dead(&server->players[i])) {
            remove_dead_player(server, i);
            i--;
        }
    }
}
