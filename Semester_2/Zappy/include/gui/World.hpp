/*
** EPITECH PROJECT, 2026
** World.hpp
** File description:
** Global game state
*/
#pragma once
#include "Tile.hpp"
#include "Player.hpp"
#include "Egg.hpp"
#include <unordered_map>
#include <vector>
#include <string>

struct World {
    int  width       = 0;
    int  height      = 0;
    int  timeUnit    = 100;
    bool gameOver    = false;

    std::vector<std::vector<Tile>>  tiles;    // tiles[y][x]
    std::unordered_map<int, Player> players;
    std::unordered_map<int, Egg>    eggs;
    std::vector<std::string>        teams;
    std::string                     winnerTeam;
    std::string                     lastMessage;

    void  resize(int w, int h);
    Tile &tileAt(int x, int y);
    int   teamIndex(const std::string &name) const;
};
