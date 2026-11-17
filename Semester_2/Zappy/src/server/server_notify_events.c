/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_notify_events - GUI protocol events (pgt, pdr, pdi, pic, pie,
** plv, pbc, pex, pfk, enw, ebo, bct, pin)
*/

#include <stdarg.h>
#include "../../include/server/server_internal.h"

/*
** Envoie une ligne à tous les clients GUI. Best effort : si le tampon
** d'un GUI est plein, la ligne est perdue pour lui mais la commande de
** l'IA qui l'a déclenchée ne doit pas échouer pour autant.
*/
void notify_gui(t_server *server, const char *fmt, ...)
{
    char line[BUFFER_SIZE];
    va_list ap;
    int len;

    va_start(ap, fmt);
    len = vsnprintf(line, sizeof(line), fmt, ap);
    va_end(ap);
    if (len <= 0)
        return;
    if (len >= (int)sizeof(line)) {
        len = (int)sizeof(line) - 1;
        line[len - 1] = '\n';
    }
    for (int i = 0; i < server->nb_clients; i++) {
        if (server->clients[i].state == CLIENT_STATE_GUI)
            enqueue_write(&server->clients[i], line, len);
    }
}

void notify_gui_tile(t_server *server, int x, int y)
{
    t_tile *tile = get_tile(server, x, y);

    if (!tile)
        return;
    notify_gui(server, "bct %d %d %d %d %d %d %d %d %d\n",
        wrap_coord(x, server->args->width),
        wrap_coord(y, server->args->height),
        tile->resources[RESOURCE_FOOD], tile->resources[RESOURCE_LINEMATE],
        tile->resources[RESOURCE_DERAUMERE], tile->resources[RESOURCE_SIBUR],
        tile->resources[RESOURCE_MENDIANE], tile->resources[RESOURCE_PHIRAS],
        tile->resources[RESOURCE_THYSTAME]);
}

void notify_gui_inventory(t_server *server, t_player *p)
{
    notify_gui(server, "pin %d %d %d %d %d %d %d %d %d %d\n",
        p->id, p->x, p->y,
        p->inventory[RESOURCE_FOOD], p->inventory[RESOURCE_LINEMATE],
        p->inventory[RESOURCE_DERAUMERE], p->inventory[RESOURCE_SIBUR],
        p->inventory[RESOURCE_MENDIANE], p->inventory[RESOURCE_PHIRAS],
        p->inventory[RESOURCE_THYSTAME]);
}

/* pic X Y L n n ... : tous les joueurs gelés par cette incantation. */
void notify_gui_incantation_start(t_server *server, t_player *player)
{
    char line[BUFFER_SIZE];
    int len = snprintf(line, sizeof(line), "pic %d %d %d",
        player->x, player->y, player->level);

    for (int i = 0; i < server->nb_players && len > 0
        && len < (int)sizeof(line) - 16; i++) {
        t_player *o = &server->players[i];

        if (!o->incanting || o->team_index != player->team_index
            || o->level != player->level
            || o->x != player->x || o->y != player->y)
            continue;
        len += snprintf(line + len, sizeof(line) - len, " %d", o->id);
    }
    notify_gui(server, "%s\n", line);
}
