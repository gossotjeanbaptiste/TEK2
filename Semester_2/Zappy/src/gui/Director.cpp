/*
** EPITECH PROJECT, 2026
** Director.cpp
** File description:
** Video mode: title card, cinematic camera that cuts to rituals and
** level-ups, lower-third captions, compact scoreboard and end card
*/

#include "gui/Director.hpp"
#include "gui/AppCamera.hpp"
#include "gui/Effects.hpp"
#include "gui/MapRenderer.hpp"
#include "gui/Narrator.hpp"
#include "gui/TeamColors.hpp"
#include "gui/UiText.hpp"
#include "gui/Lang.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

namespace {
    constexpr double MIN_SHOT      = 6.0;   // durée mini d'un plan
    constexpr double OVERVIEW_MAX  = 16.0;  // ensuite on va voir un joueur
    constexpr double FOLLOW_LEN    = 7.0;
    constexpr double CAPTION_LEN   = 4.5;

    const Color GOLD_C = {255, 203, 0, 255};
    const Color TEXT   = {240, 240, 246, 255};
    const Color MUTED  = {175, 180, 198, 255};

    float ease(float t) { return t * t * (3.0f - 2.0f * t); }

    float fadeWindow(double t, double len, double fade = 0.8)
    {
        if (t < 0 || t > len)
            return 0.0f;
        float a = 1.0f;
        if (t < fade)
            a = static_cast<float>(t / fade);
        if (t > len - fade)
            a = static_cast<float>((len - t) / fade);
        return ease(std::clamp(a, 0.0f, 1.0f));
    }

    using Lang::english;
    using Lang::T;

    std::string q(const std::string &t)
    {
        return english() ? "\u201C" + t + "\u201D" : "« " + t + " »";
    }

    void centered(const std::string &s, float y, float size, Color c,
                  bool bold = true)
    {
        float w = UiText::width(s, size, bold);
        UiText::drawShadow(s, (GetScreenWidth() - w) / 2.0f, y, size, c, bold);
    }
}

Director::Director()
{
    if (const char *v = std::getenv("ZAPPY_END_HOLD"))
        _endHold = std::atof(v);
}

void Director::caption(const std::string &title, const std::string &sub,
                       Color color)
{
    _captions.push_back({title, sub, color, 0.0});
    while (_captions.size() > 3)
        _captions.pop_front();
}

void Director::cut(Shot shot, AppCamera &camera, double now)
{
    _shot      = shot;
    _shotStart = now;
    float ts = MapRenderer::TILE_SIZE;
    switch (shot) {
    case Shot::Intro:
        camera.setShot(camera.mapCenter(), camera.mapRadius() * 1.25f, 62.0f);
        camera.setAutoOrbit(6.0f);
        break;
    case Shot::Overview:
    case Shot::Ending:
        camera.setShot(camera.mapCenter(), camera.mapRadius() * 0.9f, 48.0f);
        camera.setAutoOrbit(4.0f);
        break;
    case Shot::Ritual:
        camera.setShot({_ritualX * ts, 0.0f, _ritualY * ts}, 9.0f, 32.0f);
        camera.setAutoOrbit(12.0f);
        break;
    case Shot::Follow:
        camera.setAutoOrbit(8.0f);
        break;
    }
}

void Director::update(const World &world, const Narrator &narrator,
                      const MapRenderer &renderer, AppCamera &camera)
{
    if (world.width <= 0)
        return;
    double now = GetTime();
    if (_origin < 0) {
        _origin = now;
        _introLen = 6.0;
        cut(Shot::Intro, camera, now);
    }
    double inShot = now - _shotStart;

    // Fin de partie : plan large, puis carton de fin, puis fermeture.
    if (world.gameOver) {
        if (_endAt < 0) {
            _endAt = now;
            cut(Shot::Ending, camera, now);
        }
        if (_endHold > 0 && now - _endAt > _endHold + 1.5)
            _exit = true;
        return;
    }

    // Nouveaux événements : sous-titres + éventuelle coupe sur un rituel.
    for (auto it = narrator.spotlights().rbegin();
         it != narrator.spotlights().rend(); ++it) {
        const Spotlight &s = *it;
        if (s.seq <= _seenSeq)
            continue;
        _seenSeq = s.seq;
        char buf[128];
        if (s.type == FxType::IncantStart) {
            std::snprintf(buf, sizeof(buf),
                          T("%s · niveau %d → %d", "%s · level %d → %d"),
                          q(s.team).c_str(), s.value, s.value + 1);
            bool canCut = _shot != Shot::Ritual && _shot != Shot::Intro
                       && inShot > MIN_SHOT * 0.5;
            if (canCut) {
                _ritualX = s.x;
                _ritualY = s.y;
                _ritualDone = false;
                cut(Shot::Ritual, camera, now);
                caption(T("Rituel d'élévation", "Elevation ritual"), buf,
                        Effects::levelColor(s.value + 1));
            }
        } else if (s.type == FxType::IncantEnd && _shot == Shot::Ritual
                   && s.x == _ritualX && s.y == _ritualY) {
            _ritualDone = true;
            _ritualEnd  = now;
            if (s.value == 1)
                caption(T("Rituel réussi !", "Ritual succeeded!"),
                        T("les pierres sont consommées, chaque participant "
                          "gagne un niveau",
                          "stones are consumed, every participant levels up"),
                        {90, 220, 120, 255});
            else
                caption(T("Rituel raté", "Ritual failed"),
                        T("il manquait des pierres ou des joueurs",
                          "stones or players were missing"),
                        {255, 95, 95, 255});
        } else if (s.type == FxType::LevelUp) {
            std::string team;
            int best = narrator.bestLevelOverall(&team);
            if (best > _bestShown) {
                _bestShown = best;
                std::snprintf(buf, sizeof(buf),
                          T("%s atteint le niveau %d", "%s reaches level %d"),
                              q(team).c_str(), best);
                caption(best >= 8 ? T("Niveau maximum !", "Max level!")
                                  : T("Nouveau record", "New record"),
                        buf, Effects::levelColor(best));
            }
        } else if (s.type == FxType::EggHatch && _shot == Shot::Overview
                   && narrator.stats().hatched <= 3) {
            caption(T("Naissance", "Birth"),
                    T("un œuf éclot : un nouveau joueur rejoint son équipe",
                      "an egg hatches: a new player joins the team"),
                    {240, 230, 200, 255});
        }
    }

    switch (_shot) {
    case Shot::Intro:
        if (inShot > _introLen) {
            cut(Shot::Overview, camera, now);
            caption(T("Le monde de Zappy", "The world of Zappy"),
                    T("des IA en équipes récoltent, pondent et s'élèvent "
                      "jusqu'au niveau 8",
                      "AI teams gather, lay eggs and rise to level 8"),
                    GOLD_C);
        }
        break;
    case Shot::Ritual:
        if ((_ritualDone && now - _ritualEnd > 2.5) || inShot > 14.0)
            cut(Shot::Overview, camera, now);
        break;
    case Shot::Overview:
        if (inShot > OVERVIEW_MAX && !world.players.empty()) {
            // Plan rapproché sur un joueur au hasard.
            std::vector<int> ids;
            for (const auto &pair : world.players)
                if (pair.second.alive)
                    ids.push_back(pair.first);
            if (!ids.empty()) {
                _followId = ids[std::rand() % ids.size()];
                cut(Shot::Follow, camera, now);
                const Player &p = world.players.at(_followId);
                char buf[96];
                std::snprintf(buf, sizeof(buf),
                          T("%s · niveau %d", "%s · level %d"),
                              q(p.team).c_str(), p.level);
                caption(T("Au plus près d'un joueur", "Up close with a player"), buf,
                        teamColorFromIndex(world.teamIndex(p.team)));
            }
        }
        break;
    case Shot::Follow: {
        auto it = world.players.find(_followId);
        if (it == world.players.end() || inShot > FOLLOW_LEN) {
            cut(Shot::Overview, camera, now);
            break;
        }
        Vector2 v = renderer.getPlayerVisualPos(_followId, it->second.x,
                                                it->second.y);
        float ts = MapRenderer::TILE_SIZE;
        camera.setShot({v.x * ts, 0.0f, v.y * ts}, 8.0f, 30.0f);
        break;
    }
    case Shot::Ending:
        break;
    }
}

void Director::drawTitleCard(const World &world, float a) const
{
    float sw = static_cast<float>(GetScreenWidth());
    float sh = static_cast<float>(GetScreenHeight());
    float k  = sh / 720.0f;
    DrawRectangleGradientV(0, 0, static_cast<int>(sw), static_cast<int>(sh),
                           {5, 8, 20, static_cast<unsigned char>(210 * a)},
                           {5, 8, 20, static_cast<unsigned char>(120 * a)});
    Color g = ColorAlpha(GOLD_C, a), t = ColorAlpha(TEXT, a),
          m = ColorAlpha(MUTED, a);
    centered("ZAPPY", sh * 0.26f, 120 * k, g);
    centered(T("Un jeu en réseau où des IA s'affrontent en équipes",
               "A networked game where AI teams compete"),
             sh * 0.26f + 140 * k, 28 * k, t, false);
    centered(T("serveur en C  ·  interface 3D en C++ / raylib  ·  IA en Python",
               "C server  ·  3D C++ / raylib interface  ·  Python AI"),
             sh * 0.26f + 182 * k, 19 * k, m, false);

    // Équipes en présence.
    float y = sh * 0.66f;
    float total = 0;
    std::vector<std::string> teams = world.teams;
    for (const auto &tn : teams)
        total += UiText::width(tn, 26 * k, true) + 60 * k;
    float x = (sw - total) / 2;
    for (const auto &tn : teams) {
        Color c = ColorAlpha(teamColorFromIndex(world.teamIndex(tn)), a);
        DrawCircle(static_cast<int>(x + 12 * k), static_cast<int>(y + 15 * k),
                   10 * k, c);
        UiText::drawShadow(tn, x + 30 * k, y, 26 * k, t, true);
        x += UiText::width(tn, 26 * k, true) + 60 * k;
    }
    centered(T("Objectif : 6 joueurs au niveau 8",
               "Goal: 6 players at level 8"), y + 56 * k, 20 * k, m, false);
}

void Director::drawEndCard(const World &world, const Narrator &narrator,
                           float a) const
{
    float sw = static_cast<float>(GetScreenWidth());
    float sh = static_cast<float>(GetScreenHeight());
    float k  = sh / 720.0f;
    DrawRectangle(0, 0, static_cast<int>(sw), static_cast<int>(sh),
                  {5, 8, 20, static_cast<unsigned char>(185 * a)});
    Color tc = ColorAlpha(teamColorFromIndex(
        world.teamIndex(world.winnerTeam)), a);
    centered(T("VICTOIRE", "VICTORY"), sh * 0.22f, 96 * k,
             ColorAlpha(GOLD_C, a));
    centered(q(world.winnerTeam), sh * 0.22f + 112 * k, 56 * k, tc);

    const Narrator::Stats &s = narrator.stats();
    char line[200];
    std::snprintf(line, sizeof(line),
                  T("6 joueurs au niveau 8 en %s", "6 players at level 8 in %s"),
                  Narrator::formatDuration(narrator.elapsed()).c_str());
    centered(line, sh * 0.22f + 196 * k, 26 * k, ColorAlpha(TEXT, a), false);
    std::snprintf(line, sizeof(line),
                  T("%d rituels réussis  ·  %d œufs pondus  ·  %d objets "
                    "ramassés  ·  %d messages radio",
                    "%d successful rituals  ·  %d eggs laid  ·  %d items "
                    "picked up  ·  %d radio messages"),
                  s.ritualsOk, s.eggs, s.pickups, s.broadcasts);
    centered(line, sh * 0.22f + 240 * k, 19 * k, ColorAlpha(MUTED, a), false);
    centered("Zappy  ·  Epitech 2026", sh - 70 * k, 18 * k,
             ColorAlpha(MUTED, a), false);
}

void Director::drawScoreboard(const World &world,
                              const Narrator &narrator) const
{
    float sw = static_cast<float>(GetScreenWidth());
    float k  = GetScreenHeight() / 720.0f;
    float w = 330 * k, rowH = 30 * k;
    float h = 40 * k + rowH * world.teams.size();
    float x = sw - w - 20 * k, y = 20 * k;
    DrawRectangleRounded({x, y, w, h}, 0.15f, 8, {10, 12, 24, 190});
    UiText::draw(T("NIVEAU MAX", "MAX LEVEL"), x + 14 * k, y + 10 * k,
                 13 * k, GOLD_C, true);
    // Colonne de droite : combien de joueurs ont atteint le niveau max de
    // l'équipe (les autres sont des retardataires), puis l'objectif final
    // « n / 6 au niveau 8 » une fois ce niveau atteint.
    const char *head = T("JOUEURS", "PLAYERS");
    UiText::draw(head, x + w - 14 * k - UiText::width(head, 13 * k, true),
                 y + 10 * k, 13 * k, GOLD_C, true);
    y += 34 * k;
    for (const std::string &tn : world.teams) {
        int alive = 0, best = 0, atBest = 0;
        for (const auto &pair : world.players) {
            const Player &p = pair.second;
            if (!p.alive || p.team != tn)
                continue;
            alive++;
            best = std::max(best, p.level);
        }
        for (const auto &pair : world.players) {
            const Player &p = pair.second;
            if (p.alive && p.team == tn && p.level == best)
                atBest++;
        }
        Color tc = teamColorFromIndex(world.teamIndex(tn));
        DrawCircle(static_cast<int>(x + 20 * k), static_cast<int>(y + 9 * k),
                   7 * k, tc);
        UiText::draw(tn, x + 34 * k, y, 17 * k, TEXT, true);
        // Pastilles de niveau 1 → 8.
        float px = x + 112 * k;
        for (int l = 1; l <= 8; ++l) {
            Color c = l <= best ? Effects::levelColor(l)
                                : Color{60, 62, 80, 255};
            DrawRectangleRounded({px + (l - 1) * 9 * k, y + 3 * k, 7 * k,
                                  12 * k}, 0.4f, 4, c);
        }
        char info[48];
        if (best >= 8)
            std::snprintf(info, sizeof(info), T("★ %d/6 au niv. 8",
                                                "★ %d/6 at lvl 8"), atBest);
        else if (best > 0)
            std::snprintf(info, sizeof(info), T("%d/%d au niv. %d",
                                                "%d/%d at lvl %d"),
                          atBest, alive, best);
        else
            std::snprintf(info, sizeof(info), "—");
        float iw = UiText::width(info, 15 * k, true);
        UiText::draw(info, x + w - 14 * k - iw, y + 1 * k, 15 * k,
                     best >= 8 ? GOLD_C : TEXT, true);
        y += rowH;
    }
    (void)narrator;
}

void Director::drawCaption() const
{
    if (_captions.empty())
        return;
    const Caption &c = _captions.front();
    double t = GetTime() - c.start;
    float a = fadeWindow(t, CAPTION_LEN, 0.5);
    if (a <= 0.0f)
        return;
    float sh = static_cast<float>(GetScreenHeight());
    float k  = sh / 720.0f;
    float slide = (1.0f - a) * 30 * k;
    float x = 40 * k - slide, y = sh - 140 * k;
    float w = std::max(UiText::width(c.title, 32 * k, true),
                       UiText::width(c.sub, 20 * k)) + 50 * k;
    DrawRectangleRounded({x, y, w, 96 * k}, 0.18f, 8,
                         {8, 10, 22, static_cast<unsigned char>(200 * a)});
    DrawRectangleRec({x, y + 12 * k, 6 * k, 72 * k}, ColorAlpha(c.color, a));
    UiText::drawShadow(c.title, x + 24 * k, y + 14 * k, 32 * k,
                       ColorAlpha(TEXT, a), true);
    UiText::draw(c.sub, x + 24 * k, y + 56 * k, 20 * k,
                 ColorAlpha(c.color, a));
}

void Director::draw(const World &world, const Narrator &narrator)
{
    if (world.width <= 0 || _origin < 0)
        return;
    double now = GetTime();

    // File de sous-titres : un à la fois.
    if (!_captions.empty()) {
        if (_captions.front().start == 0.0)
            _captions.front().start = now;
        else if (now - _captions.front().start > CAPTION_LEN)
            _captions.pop_front();
        if (!_captions.empty() && _captions.front().start == 0.0)
            _captions.front().start = now;
    }

    double t = now - _origin;
    if (t < _introLen + 1.0) {
        drawTitleCard(world, fadeWindow(t + 0.8, _introLen + 1.6, 1.0));
        return;
    }
    if (_endAt >= 0 && now - _endAt > 1.5) {
        float a = ease(std::clamp(static_cast<float>(now - _endAt - 1.5),
                                  0.0f, 1.0f));
        drawEndCard(world, narrator, a);
        return;
    }

    // Habillage : logo + chrono en haut à gauche.
    float k = GetScreenHeight() / 720.0f;
    UiText::drawShadow("ZAPPY", 24 * k, 18 * k, 34 * k, GOLD_C, true);
    UiText::drawShadow(Narrator::formatDuration(narrator.elapsed()),
                       26 * k, 58 * k, 18 * k, TEXT, false);
    drawScoreboard(world, narrator);
    drawCaption();
}
