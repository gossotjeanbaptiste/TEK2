/*
** EPITECH PROJECT, 2026
** InfoPanel.hpp
** File description:
** On-screen panels: whole-map overview and hovered tile details
*/

#pragma once

#include <raylib.h>
#include "gui/World.hpp"

// Deux fenêtres incrustées en haut à gauche de l'écran :
//  - "CARTE" : vue d'ensemble (ressources totales, IA, équipes, IA par
//    équipe, oeufs) ;
//  - juste en dessous, "CASE" : détail de la case survolée par la souris
//    (ressources, joueurs présents, oeufs, incantation), affichée seulement
//    quand une case est survolée.
// La touche I masque/affiche les deux.
class InfoPanel {
public:
    void toggle() { _visible = !_visible; }

    // À appeler chaque frame avant le rendu 3D : calcule la case survolée
    // (utilisée aussi par le renderer pour la surligner).
    // `mouseFree` = faux si la souris est sur un panneau du HUD.
    void updateHover(const World &world, const Camera3D &camera, bool mouseFree);

    void draw(const World &world);

    bool hasHover() const { return _hoverX >= 0; }
    int  hoverX() const { return _hoverX; }
    int  hoverY() const { return _hoverY; }

    // Vrai si la souris est au-dessus d'un des deux panneaux : la caméra
    // doit alors ignorer clics et molette.
    bool containsMouse() const;

    // Zone du panneau CARTE (vide s'il est masqué) : ciblée par le tutoriel.
    Rectangle overviewRect() const { return _visible ? _overview : Rectangle{}; }
    bool      visible() const { return _visible; }
    void      setVisible(bool v) { _visible = v; }

private:
    int  drawOverview(const World &world, int x, int y);
    void drawHoveredTile(const World &world, int x, int y);

    bool      _visible   = true;
    int       _hoverX    = -1;
    int       _hoverY    = -1;
    Rectangle _overview  = {0, 0, 0, 0};
    Rectangle _tileRect  = {0, 0, 0, 0};
};
