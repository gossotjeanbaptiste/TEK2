/*
** EPITECH PROJECT, 2026
** Tile.hpp
** File description:
** Map tile data with incantation tracking
*/
#pragma once

enum Resource {
    FOOD = 0,
    LINEMATE,
    DERAUMERE,
    SIBUR,
    MENDIANE,
    PHIRAS,
    THYSTAME,
    RESOURCE_COUNT
};

struct Tile {
    int x                         = 0;
    int y                         = 0;
    int resources[RESOURCE_COUNT] = {};

    bool isIncanting              = false;
    int  incantationLevel         = 0;
};