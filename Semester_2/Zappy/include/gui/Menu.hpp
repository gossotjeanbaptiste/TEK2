/*
** EPITECH PROJECT, 2026
** Menu.hpp
** File description:
** HUD Menu class to display global resources on the map
*/

#pragma once

#include <raylib.h>
#include <set>
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

    // Dessine le HUD et traite les clics (dépliage des équipes, choix du
    // joueur à suivre) : non-const car il porte cet état d'interface.
    void draw(const World &world);

    // Joueur suivi par la caméra (-1 : aucun).
    int  watchedPlayerId() const { return _watchId; }
    void clearWatch() { _watchId = -1; }

    // Vrai si la souris est au-dessus d'un panneau du HUD : la caméra doit
    // alors ignorer clics et molette.
    bool isMouseCaptured() const;

    // Zone du panneau (vide s'il est fermé) et marge à droite, pour ne pas
    // recouvrir le panneau d'explications.
    Rectangle teamsRect() const { return _active ? _teamsRect : Rectangle{}; }
    void      setRightMargin(int px) { _rightMargin = px; }

private:
    void drawTeams(const World &world, int posX, int posY);

    bool _active;

    // État du panneau "Équipes & joueurs"
    std::set<std::string> _expandedTeams;
    int                   _watchId    = -1;
    Rectangle             _teamsRect  = {0, 0, 0, 0}; // hitbox du panneau
    int                   _rightMargin = 20;
};
