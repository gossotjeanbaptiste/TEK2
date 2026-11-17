/*
** EPITECH PROJECT, 2026
** Director.hpp
** File description:
** Video mode: title card, cinematic camera that cuts to rituals and
** level-ups, lower-third captions, compact scoreboard and end card
*/

#pragma once

#include <raylib.h>
#include <deque>
#include <string>
#include "gui/World.hpp"

class AppCamera;
class MapRenderer;
class Narrator;

class Director {
public:
    // Durée du carton de titre, puis du carton de fin avant fermeture
    // (ZAPPY_END_HOLD, en secondes ; 0 = rester ouvert).
    Director();

    // Après camera.update : choisit le plan et les sous-titres.
    void update(const World &world, const Narrator &narrator,
                const MapRenderer &renderer, AppCamera &camera);
    // Surcouches 2D (après EndMode3D).
    void draw(const World &world, const Narrator &narrator);
    // Vrai quand le carton de fin a assez duré : main ferme la fenêtre.
    bool wantsExit() const { return _exit; }

private:
    enum class Shot { Intro, Overview, Ritual, Follow, Ending };

    struct Caption {
        std::string title;
        std::string sub;
        Color       color;
        double      start = 0.0;
    };

    void cut(Shot shot, AppCamera &camera, double now);
    void caption(const std::string &title, const std::string &sub,
                 Color color);
    void drawTitleCard(const World &world, float alpha) const;
    void drawEndCard(const World &world, const Narrator &narrator,
                     float alpha) const;
    void drawScoreboard(const World &world, const Narrator &narrator) const;
    void drawCaption() const;

    double  _origin     = -1.0;   // première frame avec la carte
    double  _shotStart  = 0.0;
    double  _endAt      = -1.0;
    double  _endHold    = 10.0;
    double  _introLen   = 6.0;
    bool    _exit       = false;
    Shot    _shot       = Shot::Intro;
    int     _seenSeq    = 0;
    int     _ritualX    = 0;
    int     _ritualY    = 0;
    bool    _ritualDone = false;
    double  _ritualEnd  = 0.0;
    int     _followId   = -1;
    int     _bestShown  = 1;
    std::deque<Caption> _captions;
};
