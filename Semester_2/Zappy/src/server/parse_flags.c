/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** parse_flags
*/

#include "../../include/server/server_internal.h"

static int parse_flag_p(char **argv, int i, args_t *args)
{
    int val;

    if (!str_to_positive_int(argv[i], &val) || val > 65535) {
        fprintf(stderr, "Error: invalid port '%s'.\n", argv[i]);
        return 0;
    }
    args->port = val;
    return 1;
}

static int parse_flag_xy(char **argv, int i, int *dest, char flag)
{
    int val;

    if (!str_to_positive_int(argv[i], &val)) {
        fprintf(stderr, "Error: invalid -%c value '%s'.\n", flag, argv[i]);
        return 0;
    }
    *dest = val;
    return 1;
}

static int parse_flag_c(char **argv, int i, args_t *args)
{
    int val;

    if (!str_to_positive_int(argv[i], &val)) {
        fprintf(stderr, "Error: invalid -c value '%s'.\n", argv[i]);
        return 0;
    }
    args->clients_nb = val;
    return 1;
}

static int parse_flag_f(char **argv, int i, args_t *args)
{
    int val;

    if (!str_to_positive_int(argv[i], &val)) {
        fprintf(stderr, "Error: invalid -f value '%s'.\n", argv[i]);
        return 0;
    }
    args->freq = val;
    return 1;
}

static int parse_flag_n(argv_cursor_t *cursor, args_t *args, int *skip)
{
    int count = 0;

    while (cursor->index < cursor->argc &&
        cursor->argv[cursor->index][0] != '-') {
        if (!add_team(args, cursor->argv[cursor->index]))
            return 0;
        cursor->index++;
        count++;
        (*skip)++;
    }
    if (count == 0) {
        fprintf(stderr, "Error: -n requires at least one team name.\n");
        return 0;
    }
    return 1;
}

int parse_flag(argv_cursor_t *cursor, args_t *args, int *skip)
{
    char flag = cursor->argv[cursor->index - 1][1];

    if (flag == 'p')
        return parse_flag_p(cursor->argv, cursor->index, args);
    if (flag == 'x')
        return parse_flag_xy(cursor->argv, cursor->index, &args->width, 'x');
    if (flag == 'y')
        return parse_flag_xy(cursor->argv, cursor->index, &args->height, 'y');
    if (flag == 'c')
        return parse_flag_c(cursor->argv, cursor->index, args);
    if (flag == 'f')
        return parse_flag_f(cursor->argv, cursor->index, args);
    if (flag == 'n')
        return parse_flag_n(cursor, args, skip);
    fprintf(stderr, "Error: unknown flag '-%c'.\n", flag);
    return 0;
}
