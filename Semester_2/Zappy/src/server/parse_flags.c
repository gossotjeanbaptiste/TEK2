/*
** EPITECH PROJECT, 2026
** zappy
** File description:
** parse_flags
*/

#include "../../include/server/zappy_serv.h"

extern int str_to_positive_int(char *str, int *out);
extern int add_team(t_args *args, char *name);

static int parse_flag_p(char **argv, int i, t_args *args)
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

static int parse_flag_c(char **argv, int i, t_args *args)
{
    int val;

    if (!str_to_positive_int(argv[i], &val)) {
        fprintf(stderr, "Error: invalid -c value '%s'.\n", argv[i]);
        return 0;
    }
    args->clients_nb = val;
    return 1;
}

static int parse_flag_f(char **argv, int i, t_args *args)
{
    int val;

    if (!str_to_positive_int(argv[i], &val)) {
        fprintf(stderr, "Error: invalid -f value '%s'.\n", argv[i]);
        return 0;
    }
    args->freq = val;
    return 1;
}

static int parse_flag_n(char **argv, int i, int argc, t_args *args, int *skip)
{
    int count = 0;

    while (i < argc && argv[i][0] != '-') {
        if (!add_team(args, argv[i]))
            return 0;
        i++;
        count++;
    }
    if (count == 0) {
        fprintf(stderr, "Error: -n requires at least one team name.\n");
        return 0;
    }
    *skip = count - 1;
    return 1;
}

int parse_flag(char **argv, int i, int argc, t_args *args, int *skip)
{
    char flag = argv[i - 1][1];

    if (flag == 'p') return parse_flag_p(argv, i, args);
    if (flag == 'x') return parse_flag_xy(argv, i, &args->width, 'x');
    if (flag == 'y') return parse_flag_xy(argv, i, &args->height, 'y');
    if (flag == 'c') return parse_flag_c(argv, i, args);
    if (flag == 'f') return parse_flag_f(argv, i, args);
    if (flag == 'n') return parse_flag_n(argv, i, argc, args, skip);
    fprintf(stderr, "Error: unknown flag '-%c'.\n", flag);
    return 0;
}
