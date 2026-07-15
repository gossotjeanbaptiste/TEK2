/*
** EPITECH PROJECT, 2026
** Menu.hpp
** File description:
** HUD Menu class to display global resources on the map
*/

#pragma once

#include <raylib.h>
#include <string>
#include <unordered_map>
#include "gui/World.hpp"
#include "gui/Tile.hpp"

class Menu {
public:
    Menu();
    ~Menu() = default;

    void toggle();
    bool isActive() const;

    void draw(const World &world, const Color resourceColors[], const Camera3D &camera) const;

private:
    void drawGlobalResources(const World &world, const Color resourceColors[], int posX, int posY) const;
    void drawTeams(const World &world, int posX, int posY) const;
    void drawTileInspector(const World &world, const Color resourceColors[], const Camera3D &camera, int posX, int posY) const;

    bool _active;
};