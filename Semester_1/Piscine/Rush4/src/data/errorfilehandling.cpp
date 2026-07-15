/*
** EPITECH PROJECT, 2026
** G-PDG-300-NCY-3-1-PDGRUSH4-6
** File description:
** data
*/

#include "../../include/data.hpp"

int error_file(std::ifstream &file)
{
    if (!file.is_open())
        return 84;
    return 0;
}

int error_dir(DIR *dir)
{
    if (dir == nullptr) {
        std::cerr << "Error while opening the folder." << std::endl;
        return 84;
    }
    return 0;
}

int error_handling(std::ifstream &file, DIR *dir)
{
    if (!file.is_open())
        return 84;
    if (dir == nullptr) {
        std::cerr << "Error while opening the folder." << std::endl;
        return 84;
    }
    return 0;
}
