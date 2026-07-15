/*
** EPITECH PROJECT, 2026
** Player.hpp
** File description:
** Player data
*/
#pragma once
#include "Tile.hpp"
#include <string>

enum Orientation {
    NORTH = 1,
    EAST  = 2,
    SOUTH = 3,
    WEST  = 4
};

struct Player {
    int         id                        = 0;
    int         x                         = 0;
    int         y                         = 0;
    Orientation orientation               = NORTH;
    int         level                     = 1;
    std::string team;
    int         inventory[RESOURCE_COUNT] = {};
    bool        alive                     = true;
};
