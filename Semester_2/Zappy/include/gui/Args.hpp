/*
** EPITECH PROJECT, 2026
** Args.hpp
** File description:
** CLI argument parsing for zappy_gui
*/
#pragma once
#include <string>

struct Args {
    std::string host;
    int         port;
};

Args parseArgs(int argc, char **argv);
