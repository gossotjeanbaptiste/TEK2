/*
** EPITECH PROJECT, 2026
** Tutorial.cpp
** File description:
** Guided tour: pop-ups that highlight each part of the window, then wait
** for real game moments (pickup, egg, ritual...) and frame them
*/

#include "gui/Tutorial.hpp"
#include "gui/AppCamera.hpp"
#include "gui/Keys.hpp"
#include "gui/MapRenderer.hpp"
#include "gui/Narrator.hpp"
#include "gui/UiText.hpp"
#include "gui/Lang.hpp"
#include <raymath.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace {
    constexpr float BUBBLE_W   = 470.0f;
    constexpr float PAD        = 20.0f;
    constexpr float RECENT_S   = 6.0;    // un événement « récent » compte

    const Color BG      = {16, 18, 32, 245};
    const Color ACCENT  = {255, 203, 0, 255};
    const Color TEXT    = {238, 238, 244, 255};
    const Color MUTED   = {160, 165, 185, 255};
    const Color OK      = {90, 220, 120, 255};

    float pulse(float speed = 4.0f)
    {
        return 0.5f + 0.5f * sinf(static_cast<float>(GetTime()) * speed);
    }
}

Tutorial::Tutorial()
{
    buildSteps();
}

void Tutorial::buildSteps()
{
    using Lang::T;
    _builtEnglish = Lang::english();
    _steps = {
        {T("Bienvenue dans Zappy", "Welcome to Zappy"),
         T("Zappy est un jeu en réseau. Un serveur écrit en C simule un "
           "monde, des IA en Python y jouent en équipes, et cette fenêtre "
           "(C++ / raylib) affiche la partie en direct.\n\nCe tutoriel fait "
           "le tour de l'écran, puis vous montre les moments clés de la "
           "partie dès qu'ils se produisent.",
           "Zappy is a networked game. A server written in C simulates a "
           "world, Python AIs play in it as teams, and this window (C++ / "
           "raylib) shows the game live.\n\nThis tutorial walks you around "
           "the screen, then shows you the key moments of the game as soon "
           "as they happen."),
         Target::None},
        {T("Le monde", "The world"),
         T("Une carte en tuiles qui boucle sur elle-même : sortir par la "
           "droite fait revenir par la gauche. Des ressources y apparaissent "
           "en continu : de la nourriture (le jambon) et six sortes de "
           "pierres précieuses, chacune de sa couleur.",
           "A tiled map that wraps around: leaving on the right brings you "
           "back on the left. Resources keep appearing on it: food (the "
           "ham) and six kinds of precious stones, each with its own "
           "colour."),
         Target::Map},
        {T("La caméra", "The camera"),
         T("Essayez : clic gauche + glisser pour tourner, molette pour "
           "zoomer, clic droit, Z Q S D (ou W A S D, ou les flèches) pour "
           "vous déplacer.",
           "Try it: left click + drag to rotate, mouse wheel to zoom, right "
           "click, W A S D (or Z Q S D, or the arrow keys) to move around."),
         Target::Map, Done::CameraMoved},
        {T("Le panneau Carte", "The Map panel"),
         T("Vue d'ensemble : les ressources présentes sur toute la carte, "
           "et le nombre de joueurs et d'œufs de chaque équipe.\n\nSurvolez "
           "une case avec la souris : son contenu détaillé s'affiche juste "
           "en dessous. Touche I pour masquer ce panneau.",
           "Overview: the resources on the whole map, and the number of "
           "players and eggs of each team.\n\nHover a tile with the mouse: "
           "its detailed content shows up right below. Press I to hide "
           "this panel."),
         Target::Info},
        {T("Les explications en direct", "Live explanations"),
         T("Ce panneau raconte la partie : le but du jeu, la progression de "
           "chaque équipe (un bâton par niveau, de 1 à 8), des compteurs "
           "d'activité et un journal des événements. Touche H pour le "
           "masquer, L pour passer en anglais.",
           "This panel tells the story of the game: the goal, each team's "
           "progress (one bar per level, from 1 to 8), activity counters "
           "and an event log. Press H to hide it, L to switch to French."),
         Target::Narrator},
        {T("Suivre un joueur", "Follow a player"),
         T("Appuyez sur M pour ouvrir la liste des équipes.",
           "Press M to open the team list."),
         Target::Map, Done::MenuOpened},
        {T("Suivre un joueur", "Follow a player"),
         T("Cliquez sur une équipe pour la déplier, puis sur un joueur : la "
           "caméra le suivra partout. Recliquez dessus pour arrêter.",
           "Click a team to expand it, then click a player: the camera will "
           "follow them everywhere. Click again to stop."),
         Target::Menu, Done::PlayerWatched},
        {T("Un joueur", "A player"),
         T("Chaque personnage est un client IA connecté au serveur. La "
           "gemme au-dessus de sa tête donne la couleur de son équipe, et la "
           "reine de la colonie porte en plus une couronne. Son apparence "
           "change à chaque niveau.",
           "Each character is an AI client connected to the server. The gem "
           "above its head shows its team colour, and the colony's queen "
           "also wears a crown. Its look changes at every level."),
         Target::Event, Done::Next, FxType::Spawn,
         T("En attente de l'arrivée d'un joueur…",
           "Waiting for a player to arrive…")},
        {T("Ramasser", "Picking up"),
         T("Le joueur ramasse un objet sur sa case (commande Take). Il doit "
           "manger pour survivre : sans nourriture dans son sac, il meurt de "
           "faim en quelques secondes.",
           "The player picks up an item on its tile (Take command). It must "
           "eat to survive: with no food in its bag, it starves within "
           "seconds."),
         Target::Event, Done::Next, FxType::Pickup,
         T("En attente d'un ramassage…", "Waiting for a pickup…")},
        {T("Pondre un œuf", "Laying an egg"),
         T("La reine lance la commande Fork : un œuf apparaît sur sa case. "
           "Un nouveau client IA pourra s'y connecter et éclore : c'est "
           "ainsi qu'une équipe grandit.",
           "The queen runs the Fork command: an egg appears on its tile. A "
           "new AI client can connect and hatch from it: that is how a team "
           "grows."),
         Target::Event, Done::Next, FxType::EggLaid,
         T("En attente d'une ponte…", "Waiting for an egg…")},
        {T("Le rituel d'élévation", "The elevation ritual"),
         T("Des joueurs de même niveau se réunissent sur une case où sont "
           "posées les bonnes pierres, puis lancent une incantation. Ils "
           "restent figés jusqu'au résultat.",
           "Players of the same level gather on a tile holding the right "
           "stones, then start an incantation. They stay frozen until the "
           "result."),
         Target::Event, Done::Next, FxType::IncantStart,
         T("En attente d'un rituel…", "Waiting for a ritual…")},
        {T("Montée de niveau !", "Level up!"),
         T("Rituel réussi : les pierres sont consommées et chaque "
           "participant gagne un niveau. Plus on monte, plus il faut de "
           "joueurs et de pierres : il en faut 6 pour passer du niveau 7 au "
           "niveau 8.",
           "Ritual succeeded: the stones are consumed and every participant "
           "gains a level. The higher you go, the more players and stones "
           "you need: 6 players to go from level 7 to level 8."),
         Target::Event, Done::Next, FxType::LevelUp,
         T("En attente d'une montée de niveau…", "Waiting for a level up…")},
        {T("À vous de jouer", "Your turn"),
         T("Le tutoriel est terminé. La partie continue : suivez la "
           "progression dans le panneau de droite. La première équipe qui a "
           "6 joueurs au niveau 8 gagne.",
           "The tutorial is over. The game goes on: follow the progress in "
           "the right-hand panel. The first team with 6 players at level 8 "
           "wins."),
         Target::None},
    };
}

void Tutorial::start()
{
    if (const char *a = std::getenv("ZAPPY_TUTORIAL_AUTO"))
        _auto = std::atof(a);
    _active = true;
    _index  = 0;
    _stepStart = GetTime();
    _eventFound = false;
    // Échap sert à « passer le tutoriel » : on évite qu'elle ferme la fenêtre.
    SetExitKey(KEY_NULL);
}

void Tutorial::finish()
{
    _active = false;
    SetExitKey(KEY_ESCAPE);
}

void Tutorial::enter(size_t index, TutorialContext &ctx)
{
    _index      = index;
    _stepStart  = GetTime();
    _eventFound = false;
    _eventPlayer = -1;
    _doneDelay  = -1.0f;
    const Step &s = _steps[_index];
    if (s.target != Target::Event) {
        // Retour à une vue d'ensemble pour les étapes « visite ».
        if (s.target == Target::None || s.target == Target::Map)
            ctx.camera.setShot(ctx.camera.mapCenter(),
                               ctx.camera.mapRadius(), 45.0f);
        else
            ctx.camera.clearShot();
    }
}

void Tutorial::next(TutorialContext &ctx)
{
    if (_index + 1 >= _steps.size()) {
        ctx.camera.clearShot();
        finish();
        return;
    }
    enter(_index + 1, ctx);
}

bool Tutorial::findEvent(TutorialContext &ctx)
{
    FxType type = _steps[_index].event;
    Spotlight s;
    if (ctx.narrator.lastOf(type, s) && s.time >= _stepStart - RECENT_S) {
        _eventFound  = true;
        _eventX      = s.x;
        _eventY      = s.y;
        // L'œuf et le rituel restent sur leur case ; les autres suivent
        // le joueur concerné.
        _eventPlayer = (type == FxType::EggLaid
                        || type == FxType::IncantStart) ? -1 : s.player;
        return true;
    }
    // Replis : plus d'arrivées une fois la colonie complète, plus de
    // pontes non plus. On montre alors un joueur / un œuf déjà là.
    if (type == FxType::Spawn) {
        for (const auto &pair : ctx.world.players) {
            if (!pair.second.alive)
                continue;
            _eventFound  = true;
            _eventPlayer = pair.first;
            _eventX      = pair.second.x;
            _eventY      = pair.second.y;
            return true;
        }
    }
    if (type == FxType::EggLaid && GetTime() - _stepStart > 3.0
        && !ctx.world.eggs.empty()) {
        const Egg &e = ctx.world.eggs.begin()->second;
        _eventFound = true;
        _eventX     = e.x;
        _eventY     = e.y;
        return true;
    }
    return false;
}

Vector2 Tutorial::eventScreenPos(TutorialContext &ctx) const
{
    float ts = MapRenderer::TILE_SIZE;
    Vector3 p = {_eventX * ts, 0.8f, _eventY * ts};
    if (_eventPlayer >= 0 && ctx.world.players.count(_eventPlayer)) {
        Vector2 v = ctx.renderer.getPlayerVisualPos(_eventPlayer, _eventX,
                                                    _eventY);
        p = {v.x * ts, 0.8f, v.y * ts};
    }
    return GetWorldToScreen(p, ctx.camera.get());
}

void Tutorial::update(TutorialContext &ctx)
{
    if (!_active)
        return;
    if (_builtEnglish != Lang::english())
        buildSteps();       // langue changée (touche L) : textes à jour
    const Step &s = _steps[_index];

    if (IsKeyPressed(KEY_ESCAPE) || _clickSkip) {
        _clickSkip = false;
        ctx.camera.clearShot();
        finish();
        return;
    }
    if ((IsKeyPressed(KEY_BACKSPACE) || _clickPrev) && _index > 0) {
        _clickPrev = false;
        enter(_index - 1, ctx);
        return;
    }
    _clickPrev = false;
    bool wantNext = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)
                 || IsKeyPressed(KEY_SPACE) || _clickNext;
    _clickNext = false;
    if (wantNext) {
        next(ctx);
        return;
    }

    // Conditions d'action : validées, puis on passe après une courte pause
    // pour que l'utilisateur voie le « ✓ ».
    if (_doneDelay < 0.0f) {
        bool done = false;
        switch (s.done) {
        case Done::Next: break;
        case Done::CameraMoved:
            done = GetMouseWheelMove() != 0.0f
                || (IsMouseButtonDown(MOUSE_BUTTON_LEFT)
                    && Vector2Length(GetMouseDelta()) > 3.0f
                    && !containsMouse())
                || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)
                || Keys::anyMove();
            break;
        case Done::MenuOpened:    done = ctx.menuOpen; break;
        case Done::PlayerWatched: done = ctx.watchedId >= 0; break;
        }
        if (done)
            _doneDelay = 1.2f;
    } else {
        _doneDelay -= GetFrameTime();
        if (_doneDelay <= 0.0f) {
            next(ctx);
            return;
        }
    }

    if (_auto > 0.0) {
        double since = (s.target == Target::Event) ? GetTime() - _foundAt
                                                   : GetTime() - _stepStart;
        bool ready = s.target != Target::Event || _eventFound;
        if (ready && since > _auto) {
            next(ctx);
            return;
        }
    }

    if (s.target == Target::Event) {
        // Rien depuis 15 s (ex. colonie déjà complète : plus de pontes) :
        // on donne quand même l'explication, sans cadrage.
        if (!_eventFound && GetTime() - _stepStart > 15.0) {
            _eventFound  = true;
            _foundAt     = GetTime();
            _eventX      = -1;
            _eventPlayer = -1;
        }
        if (!_eventFound && findEvent(ctx)) {
            _foundAt = GetTime();
            float ts = MapRenderer::TILE_SIZE;
            ctx.camera.setShot({_eventX * ts, 0.0f, _eventY * ts},
                               10.0f, 38.0f);
        }
        // Suit le joueur concerné s'il se déplace.
        if (_eventFound && _eventPlayer >= 0
            && ctx.world.players.count(_eventPlayer)) {
            const Player &p = ctx.world.players.at(_eventPlayer);
            _eventX = p.x;
            _eventY = p.y;
            if (ctx.camera.hasShot()) {
                Vector2 v = ctx.renderer.getPlayerVisualPos(_eventPlayer,
                                                            p.x, p.y);
                float ts = MapRenderer::TILE_SIZE;
                ctx.camera.setShot({v.x * ts, 0.0f, v.y * ts}, 10.0f, 38.0f);
            }
        }
    }
}

Rectangle Tutorial::targetRect(const TutorialContext &ctx) const
{
    float sw = static_cast<float>(GetScreenWidth());
    float sh = static_cast<float>(GetScreenHeight());
    switch (_steps[_index].target) {
    case Target::Info:     return ctx.infoRect;
    case Target::Narrator: return ctx.narratorRect;
    case Target::Menu:     return ctx.menuRect;
    case Target::Map: {
        float right = ctx.narratorRect.width > 0 ? ctx.narratorRect.x - 10
                                                 : sw - 10;
        float left = ctx.infoRect.width > 0
                   ? ctx.infoRect.x + ctx.infoRect.width + 10 : 10;
        return {left, 10, right - left, sh - 20};
    }
    default: return {};
    }
}

bool Tutorial::containsMouse() const
{
    return _active && CheckCollisionPointRec(GetMousePosition(), _bubble);
}

void Tutorial::drawSpotlight(Rectangle r) const
{
    float sw = static_cast<float>(GetScreenWidth());
    float sh = static_cast<float>(GetScreenHeight());
    Color dim = {0, 0, 0, 150};
    float m = 6.0f;
    Rectangle h = {r.x - m, r.y - m, r.width + 2 * m, r.height + 2 * m};
    DrawRectangleRec({0, 0, sw, h.y}, dim);
    DrawRectangleRec({0, h.y + h.height, sw, sh - h.y - h.height}, dim);
    DrawRectangleRec({0, h.y, h.x, h.height}, dim);
    DrawRectangleRec({h.x + h.width, h.y, sw - h.x - h.width, h.height}, dim);
    DrawRectangleRoundedLinesEx(h, 0.04f, 8, 3.0f,
                                ColorAlpha(ACCENT, 0.45f + 0.55f * pulse()));
}

bool Tutorial::button(Rectangle r, const std::string &label,
                      bool primary) const
{
    bool hover = CheckCollisionPointRec(GetMousePosition(), r);
    Color bg = primary ? (hover ? Color{255, 220, 70, 255} : ACCENT)
                       : (hover ? Color{60, 64, 90, 255}
                                : Color{40, 43, 64, 255});
    DrawRectangleRounded(r, 0.35f, 8, bg);
    Color fg = primary ? Color{20, 20, 30, 255} : TEXT;
    float size = 15.0f;
    UiText::draw(label, r.x + (r.width - UiText::width(label, size, true)) / 2,
                 r.y + (r.height - size) / 2 - 1, size, fg, true);
    return hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

void Tutorial::drawBubble(TutorialContext &ctx, Rectangle target,
                          bool waiting)
{
    const Step &s = _steps[_index];
    float sw = static_cast<float>(GetScreenWidth());
    float sh = static_cast<float>(GetScreenHeight());
    float tw = BUBBLE_W - 2 * PAD;

    std::string body = waiting ? s.waiting : s.body;
    if (waiting)
        body += Lang::T("\n\nLa partie continue en arrière-plan ; Entrée "
                        "pour passer cette étape.",
                        "\n\nThe game goes on in the background; press "
                        "Enter to skip this step.");
    auto lines = UiText::wrap(body, 16, tw);
    float h = PAD + 18 + 32 + lines.size() * 21.0f + 18 + 38 + 22 + PAD;

    // Placement : à côté de la zone ciblée, sinon centré / en bas.
    float x = (sw - BUBBLE_W) / 2, y = (sh - h) / 2;
    if (target.width > 0 && s.target != Target::Map) {
        if (target.x > sw / 2)
            x = target.x - BUBBLE_W - 24;
        else
            x = target.x + target.width + 24;
        y = Clamp(target.y + 20, 20, sh - h - 20);
    } else if (s.target == Target::Map || s.target == Target::Event) {
        y = sh - h - 30;
        if (ctx.narratorRect.width > 0)
            x = Clamp(x, 20, ctx.narratorRect.x - BUBBLE_W - 20);
    }
    _bubble = {x, y, BUBBLE_W, h};

    // Flèche vers l'événement cadré.
    if (s.target == Target::Event && _eventFound && _eventX >= 0) {
        Vector2 p = eventScreenPos(ctx);
        Vector2 from = {x + BUBBLE_W / 2, y};
        DrawLineEx(from, p, 3.0f, ColorAlpha(ACCENT, 0.8f));
        float r = 22.0f + 8.0f * pulse(5.0f);
        DrawCircleLines(static_cast<int>(p.x), static_cast<int>(p.y), r,
                        ACCENT);
        DrawCircleLines(static_cast<int>(p.x), static_cast<int>(p.y), r + 2,
                        ACCENT);
    }

    DrawRectangleRounded({x + 4, y + 6, BUBBLE_W, h}, 0.08f, 10,
                         {0, 0, 0, 90});
    DrawRectangleRounded(_bubble, 0.08f, 10, BG);
    DrawRectangleRoundedLinesEx(_bubble, 0.08f, 10, 2.0f,
                                ColorAlpha(ACCENT, 0.7f));

    float cy = y + PAD;
    char counter[48];
    std::snprintf(counter, sizeof(counter),
                  Lang::T("TUTORIEL · ÉTAPE %zu / %zu",
                          "TUTORIAL · STEP %zu / %zu"),
                  _index + 1, _steps.size());
    UiText::draw(counter, x + PAD, cy, 12.5f, ACCENT, true);
    // Barre de progression.
    float pw = 120.0f, px = x + BUBBLE_W - PAD - pw;
    DrawRectangleRounded({px, cy + 4, pw, 6}, 1.0f, 4, {50, 54, 78, 255});
    DrawRectangleRounded({px, cy + 4,
                          pw * (_index + 1) / static_cast<float>(_steps.size()),
                          6}, 1.0f, 4, ACCENT);
    cy += 18;
    std::string title = s.title;
    if (_doneDelay >= 0.0f)
        title += "  ✓";
    UiText::draw(title, x + PAD, cy, 24, _doneDelay >= 0.0f ? OK : TEXT,
                 true);
    if (waiting) {
        // Petit indicateur d'attente animé.
        float a = static_cast<float>(GetTime()) * 6.0f;
        float tx = x + PAD + UiText::width(title, 24, true) + 18;
        for (int i = 0; i < 3; ++i)
            DrawCircle(static_cast<int>(tx + i * 12), static_cast<int>(cy + 13),
                       3.0f + 1.5f * sinf(a - i), ACCENT);
    }
    cy += 32;
    for (const std::string &line : lines) {
        UiText::draw(line, x + PAD, cy, 16, waiting ? MUTED : TEXT);
        cy += 21;
    }
    cy += 18;

    float by = y + h - PAD - 34 - 22;
    bool last = _index + 1 >= _steps.size();
    if (button({x + BUBBLE_W - PAD - 150, by, 150, 34},
               last ? Lang::T("Terminer", "Finish")
                    : Lang::T("Suivant  →", "Next  →"), true))
        _clickNext = true;
    if (_index > 0 && button({x + BUBBLE_W - PAD - 150 - 10 - 110, by, 110, 34},
                             Lang::T("←  Retour", "←  Back"), false))
        _clickPrev = true;
    if (!last && button({x + PAD, by, 120, 34}, Lang::T("Passer", "Skip"), false))
        _clickSkip = true;
    std::string keys = Lang::T("Entrée : suivant  ·  Retour arrière : "
                               "précédent  ·  Échap : quitter le tutoriel",
                               "Enter: next  ·  Backspace: previous  ·  "
                               "Esc: leave the tutorial");
    UiText::draw(keys, x + (BUBBLE_W - UiText::width(keys, 11.5f)) / 2,
                 by + 44, 11.5f, MUTED);
}

void Tutorial::draw(TutorialContext &ctx)
{
    if (!_active)
        return;
    const Step &s = _steps[_index];
    Rectangle target = targetRect(ctx);
    if (s.target == Target::None) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                      {0, 0, 0, 140});
    } else if (target.width > 0 && s.target != Target::Event) {
        drawSpotlight(target);
    }
    bool waiting = s.target == Target::Event && !_eventFound;
    drawBubble(ctx, target, waiting);
}
