/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** parse_utils
*/

#include "../../include/server/server_internal.h"

void print_usage(char *binary)
{
    fprintf(stderr, "USAGE: %s -p port -x width -y height"
        " -n name1 name2 ... -c clientsNb -f freq\n", binary);
}

int str_to_positive_int(char *str, int *out)
{
    char *end;
    long val;

    val = strtol(str, &end, 10);
    if (*end != '\0' || end == str)
        return 0;
    if (val <= 0 || val > 2147483647)
        return 0;
    *out = (int)val;
    return 1;
}

int is_valid_port(int port)
{
    return port > 0 && port <= 65535;
}

static int is_reserved_name(char *name)
{
    return strcmp(name, "GRAPHIC") == 0;
}

static int has_duplicate_team(args_t *args, char *name)
{
    for (int i = 0; i < args->teams_count; i++) {
        if (strcmp(args->teams[i], name) == 0)
            return 1;
    }
    return 0;
}

int add_team(args_t *args, char *name)
{
    if (is_reserved_name(name)) {
        fprintf(stderr, "Error: team name 'GRAPHIC' is reserved.\n");
        return 0;
    }
    if (has_duplicate_team(args, name)) {
        fprintf(stderr, "Error: duplicate team name '%s'.\n", name);
        return 0;
    }
    if (args->teams_count >= MAX_TEAMS) {
        fprintf(stderr, "Error: too many teams (max %d).\n", MAX_TEAMS);
        return 0;
    }
    args->teams[args->teams_count] = name;
    args->teams_count++;
    return 1;
}
