/*
** EPITECH PROJECT, 2026
** Tutorial.hpp
** File description:
** Guided tour: pop-ups that highlight each part of the window, then wait
** for real game moments (pickup, egg, ritual...) and frame them
*/

#pragma once

#include <raylib.h>
#include <string>
#include <utility>
#include <vector>
#include "gui/World.hpp"

class AppCamera;
class MapRenderer;
class Narrator;

// Ce que le tutoriel doit savoir de la fenêtre à chaque frame.
struct TutorialContext {
    const World       &world;
    const Narrator    &narrator;
    const MapRenderer &renderer;
    AppCamera         &camera;
    Rectangle          infoRect;      // panneau CARTE (vide si masqué)
    Rectangle          menuRect;      // panneau ÉQUIPES (vide si fermé)
    Rectangle          narratorRect;  // panneau d'explications
    bool               menuOpen;
    int                watchedId;     // joueur suivi (-1 : aucun)
};

class Tutorial {
public:
    Tutorial();

    void start();
    bool active() const { return _active; }
    // Avance l'étape courante (clavier, souris, conditions) ; à appeler
    // après camera.update pour que le cadrage automatique s'applique.
    void update(TutorialContext &ctx);
    // À dessiner en dernier, par-dessus tout le HUD.
    void draw(TutorialContext &ctx);
    // Vrai si la souris est sur la bulle : la caméra doit l'ignorer.
    bool containsMouse() const;

private:
    enum class Target { None, Map, Info, Narrator, Menu, Event };
    enum class Done   { Next, CameraMoved, MenuOpened, PlayerWatched };

    struct Step {
        std::string title;
        std::string body;
        Target      target;
        Done        done;
        FxType      event;    // si target == Event
        std::string waiting;  // texte pendant l'attente

        Step(std::string t, std::string b, Target tg = Target::None,
             Done d = Done::Next, FxType e = FxType::Spawn,
             std::string w = "")
            : title(std::move(t)), body(std::move(b)), target(tg), done(d),
              event(e), waiting(std::move(w)) {}
    };

    // (Re)crée les étapes dans la langue courante.
    void buildSteps();
    void enter(size_t index, TutorialContext &ctx);
    void next(TutorialContext &ctx);
    void finish();
    bool findEvent(TutorialContext &ctx);
    Rectangle targetRect(const TutorialContext &ctx) const;
    Vector2   eventScreenPos(TutorialContext &ctx) const;
    void drawSpotlight(Rectangle r) const;
    void drawBubble(TutorialContext &ctx, Rectangle target, bool waiting);
    bool button(Rectangle r, const std::string &label, bool primary) const;

    std::vector<Step> _steps;
    size_t    _index      = 0;
    bool      _active     = false;
    double    _stepStart  = 0.0;
    bool      _eventFound = false;
    int       _eventX     = 0;
    int       _eventY     = 0;
    int       _eventPlayer = -1;
    float     _doneDelay  = -1.0f;  // petite pause après une action réussie
    Rectangle _bubble     = {};
    bool      _clickNext  = false;
    bool      _clickPrev  = false;
    bool      _clickSkip  = false;
    double    _foundAt    = 0.0;
    // ZAPPY_TUTORIAL_AUTO=<s> : avance seul (écran sans surveillance).
    double    _auto       = 0.0;
    bool      _builtEnglish = false;
};
