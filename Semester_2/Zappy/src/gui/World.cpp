/*
** EPITECH PROJECT, 2026
** World.cpp
** File description:
** Global game state
*/

#include "gui/World.hpp"

void World::resize(int w, int h)
{
    width  = w;
    height = h;
    tiles.assign(h, std::vector<Tile>(w));
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            tiles[y][x].x = x;
            tiles[y][x].y = y;
        }
}

Tile &World::tileAt(int x, int y)
{
    x = ((x % width) + width) % width;
    y = ((y % height) + height) % height;
    return tiles[y][x];
}

int World::teamIndex(const std::string &name) const
{
    for (int i = 0; i < static_cast<int>(teams.size()); ++i)
        if (teams[i] == name) return i;
    return -1;
}
