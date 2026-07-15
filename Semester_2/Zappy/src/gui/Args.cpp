/*
** EPITECH PROJECT, 2026
** Args.cpp
** File description:
** CLI argument parsing for zappy_gui
*/

#include "gui/Args.hpp"
#include <cstdlib>
#include <stdexcept>

static void printUsage(const char *bin)
{
    throw std::runtime_error(
        std::string("USAGE: ") + bin + " -p port -h machine\n"
        "  -p port     port number of the server\n"
        "  -h machine  hostname of the server"
    );
}

Args parseArgs(int argc, char **argv)
{
    Args args{"localhost", 0};
    bool hasPort = false;
    bool hasHost = false;

    for (int i = 1; i < argc; ++i) {
        std::string flag(argv[i]);
        if (flag == "-p") {
            if (i + 1 >= argc)
                printUsage(argv[0]);
            args.port = std::atoi(argv[++i]);
            if (args.port <= 0)
                throw std::runtime_error("Invalid port number");
            hasPort = true;
        } else if (flag == "-h") {
            if (i + 1 >= argc)
                printUsage(argv[0]);
            args.host = argv[++i];
            hasHost = true;
        } else {
            printUsage(argv[0]);
        }
    }
    if (!hasPort || !hasHost)
        printUsage(argv[0]);
    return args;
}
