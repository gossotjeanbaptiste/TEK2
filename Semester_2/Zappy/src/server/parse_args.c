/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** parse_args
*/

#include "../../include/server/zappy_serv.h"

extern int parse_flag(char **argv, int i, int argc, t_args *args, int *skip);

static void init_args(t_args *args)
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

static int validate_args(t_args *args)
{
    if (args->port == -1)
        return fprintf(stderr, "Error: missing -p\n"), 0;
    if (args->width == -1)
        return fprintf(stderr, "Error: missing -x\n"), 0;
    if (!is_valid_map_dim(args->width))
        return fprintf(stderr, "Error: width must be between 1 and %d.\n", MAX_MAP_DIM), 0;
    if (args->height == -1)
        return fprintf(stderr, "Error: missing -y\n"), 0;
    if (!is_valid_map_dim(args->height))
        return fprintf(stderr, "Error: height must be between 1 and %d.\n", MAX_MAP_DIM), 0;
    if (args->clients_nb == -1)
        return fprintf(stderr, "Error: missing -c\n"), 0;
    if (args->teams_count == 0)
        return fprintf(stderr, "Error: missing -n\n"), 0;
    return 1;
}

static int is_flag(char *str)
{
    return str[0] == '-' && str[1] != '\0' && str[2] == '\0';
}

static int loop_args(int argc, char **argv, t_args *args)
{
    int skip = 0;

    for (int i = 1; i < argc; i++) {
        if (skip > 0) { skip--; continue; }
        if (!is_flag(argv[i])) {
            fprintf(stderr, "Error: unexpected token '%s'.\n", argv[i]);
            return 0;
        }
        if (i + 1 >= argc) {
            fprintf(stderr, "Error: flag '%s' missing value.\n", argv[i]);
            return 0;
        }
        if (!parse_flag(argv, i + 1, argc, args, &skip))
            return 0;
        i++;
    }
    return 1;
}

int parse_args(int argc, char **argv, t_args *args)
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
