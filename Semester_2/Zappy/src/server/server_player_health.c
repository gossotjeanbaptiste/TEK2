/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_player_health - food decay and death management
*/

#include "../../include/server/server_internal.h"

static long food_decay_interval_ms(server_t *server)
{
    long interval = 0;

    if (!server || !server->args || server->args->freq <= 0)
        return 126000;
    interval = (126L * 1000L) / server->args->freq;
    return (interval < 1) ? 1 : interval;
}

static int consume_food_from_inventory(player_t *player)
{
    if (!player || player->inventory[RESOURCE_FOOD] <= 0)
        return 0;
    player->inventory[RESOURCE_FOOD]--;
    player->food++;
    return 1;
}

int player_lose_food(server_t *server, player_t *player, int amount)
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

int is_player_dead(player_t *player)
{
    return (player && player->food <= 0) ? 1 : 0;
}

void notify_player_dead(server_t *server, client_t *client)
{
    if (!server || !client)
        return;
    if (!enqueue_write(client, "dead\n", 5))
        remove_client(server, (int)(client - server->clients));
}

int remove_dead_player(server_t *server, int player_index)
{
    player_t *player = &server->players[player_index];
    client_t *client = get_client_by_fd(server, player->client_fd);

    notify_player_dead(server, client);
    notify_gui_player_death(server, player->id);
    for (int i = player_index; i < server->nb_players - 1; i++)
        server->players[i] = server->players[i + 1];
    server->nb_players--;
    return 1;
}

void process_food_decay(server_t *server)
{
    long now;
    long interval;

    if (!server)
        return;
    now = get_now_ms();
    interval = food_decay_interval_ms(server);
    for (int i = 0; i < server->nb_players; i++) {
        if (server->players[i].next_food_decay_ms <= 0)
            server->players[i].next_food_decay_ms = now + interval;
        while (now >= server->players[i].next_food_decay_ms) {
            player_lose_food(server, &server->players[i], 1);
            server->players[i].next_food_decay_ms += interval;
        }
        if (is_player_dead(&server->players[i])) {
            remove_dead_player(server, i);
            i--;
        }
    }
}
