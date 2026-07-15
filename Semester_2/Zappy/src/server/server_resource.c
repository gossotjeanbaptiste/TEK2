/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** server_resource
*/

#include <string.h>
#include "../../include/server/server_internal.h"

int resource_index(const char *name)
{
    static const char *const names[RESOURCE_COUNT] = {
        "food", "linemate", "deraumere", "sibur",
        "mendiane", "phiras", "thystame"
    };

    for (int i = 0; i < RESOURCE_COUNT; i++)
        if (strcmp(name, names[i]) == 0)
            return i;
    return -1;
}
