/*
** EPITECH PROJECT, 2024
** Arcade
** File description:
** Main entry point for Arcade
*/

#include "Core.hpp"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    std::string graphicsLib;
    std::string gameLib = "";

    if (argc == 1) {
        graphicsLib = "./lib/arcade_ncurses.so";
    } else if (argc == 2) {
        graphicsLib = argv[1];
    } else if (argc == 3) {
        graphicsLib = argv[1];
        gameLib = argv[2];
    } else {
        std::cerr << "Usage: " << argv[0] << " [graphics_library] [game_library]" << std::endl;
        std::cerr << "Default: ./lib/arcade_ncurses.so (with menu)" << std::endl;
        return 84;
    }

    if (!fs::exists(graphicsLib)) {
        std::cerr << "Error: '" << graphicsLib << "' does not exist" << std::endl;
        return 84;
    }

    if (!gameLib.empty() && !fs::exists(gameLib)) {
        std::cerr << "Error: '" << gameLib << "' does not exist" << std::endl;
        return 84;
    }

    try {
        std::cerr << "Initializing core with graphics library: " << graphicsLib << std::endl;
        if (!gameLib.empty())
            std::cerr << "Loading initial game: " << gameLib << std::endl;
        arc::Core core(graphicsLib, gameLib);

        std::cerr << "Core initialized successfully, starting main loop..." << std::endl;
        core.run();
        return 0;
    } catch (const arc::LibLoader::exception& e) {
        std::cerr << "Error: '" << graphicsLib << "' not a graphical library" << std::endl;
        return 84;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    }

    return 0;
}
