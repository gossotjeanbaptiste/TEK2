/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** parse_args
*/

#include "../../include/server/server_internal.h"

static void init_args(args_t *args)
{
    args->port = -1;
    args->width = -1;
    args->height = -1;
    args->clients_nb = -1;
    args->freq = DEFAULT_FREQ;
    args->teams_count = 0;
    memset(args->teams, 0, sizeof(args->teams));
}

static int is_valid_map_dim(int value)
{
    return value > 0 && value <= MAX_MAP_DIM;
}

static int validate_map_dim(int value, char flag)
{
    if (value == -1) {
        fprintf(stderr, "Error: missing -%c\n", flag);
        return 0;
    }
    if (!is_valid_map_dim(value)) {
        fprintf(stderr, "Error: -%c must be between 1 and %d.\n",
            flag, MAX_MAP_DIM);
        return 0;
    }
    return 1;
}

static int validate_args(args_t *args)
{
    if (args->port == -1) {
        fprintf(stderr, "Error: missing -p\n");
        return 0;
    }
    if (!validate_map_dim(args->width, 'x'))
        return 0;
    if (!validate_map_dim(args->height, 'y'))
        return 0;
    if (args->clients_nb == -1) {
        fprintf(stderr, "Error: missing -c\n");
        return 0;
    }
    if (args->teams_count == 0) {
        fprintf(stderr, "Error: missing -n\n");
        return 0;
    }
    return 1;
}

static int is_flag(char *str)
{
    return str[0] == '-' && str[1] != '\0' && str[2] == '\0';
}

static int check_token(argv_cursor_t *cursor)
{
    char *tok = cursor->argv[cursor->index];

    if (!is_flag(tok)) {
        fprintf(stderr, "Error: unexpected token '%s'.\n", tok);
        return 0;
    }
    if (cursor->index + 1 >= cursor->argc && tok[1] != 'n') {
        fprintf(stderr, "Error: flag '%s' missing value.\n", tok);
        return 0;
    }
    return 1;
}

static int handle_flag(argv_cursor_t *cursor, args_t *args, int *skip)
{
    argv_cursor_t value_cursor = {
        cursor->argv, cursor->argc, cursor->index + 1};

    if (!parse_flag(&value_cursor, args, skip))
        return 0;
    if (cursor->argv[cursor->index][1] != 'n')
        (*skip)++;
    return 1;
}

static int loop_args(int argc, char **argv, args_t *args)
{
    int skip = 0;
    argv_cursor_t cursor = {argv, argc, 0};

    for (cursor.index = 1; cursor.index < argc; cursor.index++) {
        if (skip > 0) {
            skip--;
            continue;
        }
        if (!check_token(&cursor))
            return 0;
        if (!handle_flag(&cursor, args, &skip))
            return 0;
    }
    return 1;
}

int parse_args(int argc, char **argv, args_t *args)
{
    init_args(args);
    if (argc < 2) {
        print_usage(argv[0]);
        return 0;
    }
    if (!loop_args(argc, argv, args))
        return 0;
    if (!validate_args(args))
        return 0;
    return 1;
}
