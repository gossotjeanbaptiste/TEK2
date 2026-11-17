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

enum class AnimAction {
    Death,
    Roll,
    RollEnd,
    SwordSlash,
    Interact,
    Wave,
    KickLeft,
    KickRight,
    PunchLeft,
    PunchRight,
    HitReceiveEject
};

struct AnimEvent {
    int        playerId;
    AnimAction action;
};

// Événements visuels (particules, objets volants, textes flottants) produits
// par le parseur protocole et consommés par Effects à la frame suivante.
enum class FxType {
    Pickup,       // pgt : value = ressource
    Drop,         // pdr : value = ressource
    Death,        // pdi
    Spawn,        // pnw
    LevelUp,      // plv : value = nouveau niveau
    IncantStart,  // pic : (x, y), value = niveau des incantateurs
    IncantEnd,    // pie : (x, y), value = résultat (1 = réussite)
    Broadcast,    // pbc
    Eject,        // pex
    Fork,         // pfk
    EggLaid,      // enw : (x, y)
    EggHatch,     // ebo : (x, y)
    EggDie        // edi : (x, y)
};

struct FxEvent {
    FxType type;
    int    playerId = -1;
    int    x        = 0;
    int    y        = 0;
    int    value    = 0;
};

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
    std::vector<AnimEvent>          animEvents;
    std::vector<FxEvent>            fxEvents;

    void  resize(int w, int h);
    Tile &tileAt(int x, int y);
    int   teamIndex(const std::string &name) const;
};
