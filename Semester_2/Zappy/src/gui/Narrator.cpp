/*
** EPITECH PROJECT, 2026
** Narrator.cpp
** File description:
** In-window commentary: goal legend, team board, activity counters,
** human-readable event log and victory banner
*/

#include "gui/Narrator.hpp"
#include "gui/Effects.hpp"
#include "gui/TeamColors.hpp"
#include "gui/UiText.hpp"
#include "gui/Lang.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>

namespace {
    constexpr size_t MAX_LOG   = 60;
    constexpr size_t MAX_SPOTS = 40;
    constexpr float  PAD       = 16.0f;

    const Color BG      = {12, 14, 24, 225};
    const Color BORDER  = {255, 255, 255, 60};
    const Color TITLE   = {255, 203, 0, 255};
    const Color TEXT    = {235, 235, 240, 255};
    const Color MUTED   = {160, 165, 180, 255};
    const Color C_GOOD  = {90, 220, 120, 255};
    const Color C_BAD   = {255, 95, 95, 255};
    const Color C_RITE  = {255, 200, 60, 255};
    const Color C_EGG   = {240, 230, 200, 255};
    const Color C_INFO  = {120, 200, 255, 255};

    using Lang::T;

    const char *goal()
    {
        return T("But : la première équipe qui a 6 joueurs au niveau 8 "
                 "gagne.\nChaque IA « reine » pond des œufs. Les drones qui "
                 "en éclosent partent chercher nourriture et pierres, les "
                 "rapportent au nid, puis tous se rassemblent pour un rituel "
                 "d'élévation.",
                 "Goal: the first team with 6 players at level 8 wins.\n"
                 "Each \u201Cqueen\u201D AI lays eggs. The drones that "
                 "hatch go out for food and stones, bring them back to the "
                 "nest, then everyone gathers for an elevation ritual.");
    }

    std::string q(const std::string &team)
    {
        return Lang::english() ? "\u201C" + team + "\u201D"
                               : "« " + team + " »";
    }
}

std::string Narrator::formatDuration(double s)
{
    int total = static_cast<int>(s);
    char buf[32];
    if (total < 60)
        std::snprintf(buf, sizeof(buf), "%d s", total);
    else
        std::snprintf(buf, sizeof(buf), "%d min %02d s", total / 60,
                      total % 60);
    return buf;
}

double Narrator::elapsed() const
{
    if (!_started)
        return 0.0;
    return (_endTime >= 0 ? _endTime : GetTime()) - _start;
}

void Narrator::log(const std::string &text, Color color, Kind kind,
                   const std::string &team)
{
    _log.push_front({elapsed(), text, color, kind, team});
    while (_log.size() > MAX_LOG)
        _log.pop_back();
}

bool Narrator::mergeHatch(Kind incoming, const std::string &team)
{
    if (_log.empty() || elapsed() - _log.front().time > 1.0)
        return false;
    Entry &e = _log.front();
    bool pair = (incoming == Kind::Spawn && e.kind == Kind::Hatch)
             || (incoming == Kind::Hatch && e.kind == Kind::Spawn);
    if (!pair)
        return false;
    const std::string &who = (incoming == Kind::Spawn) ? team : e.team;
    e.text = T("Un œuf éclot : un joueur ", "An egg hatches: a ")
           + q(who) + T(" naît", " player is born");
    e.color = C_EGG;
    e.kind  = Kind::Other;
    return true;
}

void Narrator::spot(const FxEvent &ev, const std::string &team, int x, int y)
{
    Spotlight s;
    s.type   = ev.type;
    s.x      = x;
    s.y      = y;
    s.value  = ev.value;
    s.player = ev.playerId;
    s.team   = team;
    s.time   = GetTime();
    s.seq    = ++_seq;
    _spots.push_front(s);
    while (_spots.size() > MAX_SPOTS)
        _spots.pop_back();
}

bool Narrator::lastOf(FxType type, Spotlight &out) const
{
    for (const Spotlight &s : _spots) {
        if (s.type == type) {
            out = s;
            return true;
        }
    }
    return false;
}

int Narrator::bestLevel(const std::string &team) const
{
    auto it = _best.find(team);
    return it == _best.end() ? 0 : it->second;
}

int Narrator::bestLevelOverall(std::string *team) const
{
    int best = 0;
    for (const auto &pair : _best) {
        if (pair.second > best) {
            best = pair.second;
            if (team)
                *team = pair.first;
        }
    }
    return best;
}

std::string Narrator::teamOf(const World &world, int playerId) const
{
    auto it = world.players.find(playerId);
    if (it != world.players.end())
        return it->second.team;
    auto known = _knownTeam.find(playerId);
    return known == _knownTeam.end() ? "?" : known->second;
}

std::string Narrator::teamOnTile(const World &world, int x, int y,
                                 int lvl) const
{
    for (const auto &pair : world.players) {
        const Player &p = pair.second;
        if (p.alive && p.x == x && p.y == y && p.level == lvl)
            return p.team;
    }
    return "?";
}

void Narrator::observe(const World &world)
{
    if (!_started && world.width > 0) {
        _started = true;
        _start   = GetTime();
    }
    for (const auto &pair : world.players)
        _knownTeam[pair.first] = pair.second.team;

    for (const FxEvent &ev : world.fxEvents) {
        int x = ev.x, y = ev.y;
        auto pit = world.players.find(ev.playerId);
        if (pit != world.players.end() && ev.type != FxType::Death) {
            x = pit->second.x;
            y = pit->second.y;
        }
        std::string team = teamOf(world, ev.playerId);
        char buf[160];

        switch (ev.type) {
        case FxType::Spawn:
            _stats.joined++;
            if (pit != world.players.end())
                _best[team] = std::max(_best[team], ev.value);
            if (!mergeHatch(Kind::Spawn, team))
                log(T("Un joueur ", "A ") + q(team)
                    + T(" entre en jeu", " player joins the game"), C_INFO,
                    Kind::Spawn, team);
            break;
        case FxType::Death:
            _stats.deaths++;
            log(T("Un joueur ", "A ") + q(team)
                + T(" meurt de faim", " player starves to death"), C_BAD);
            break;
        case FxType::Pickup:  _stats.pickups++;    break;
        case FxType::Drop:    _stats.drops++;      break;
        case FxType::Broadcast: _stats.broadcasts++; break;
        case FxType::Fork:    break;
        case FxType::Eject:
            _stats.ejects++;
            log(T("Un joueur ", "A ") + q(team)
                + T(" éjecte ses voisins", " player ejects its neighbours"),
                MUTED);
            break;
        case FxType::EggLaid:
            _stats.eggs++;
            log(q(team) + T(" pond un œuf (futur coéquipier)",
                            " lays an egg (future teammate)"), C_EGG);
            break;
        case FxType::EggHatch:
            _stats.hatched++;
            if (!mergeHatch(Kind::Hatch, team))
                log(T("Un œuf éclot : un nouveau joueur arrive",
                      "An egg hatches: a new player arrives"), C_EGG,
                    Kind::Hatch);
            break;
        case FxType::EggDie:
            break;
        case FxType::IncantStart:
            _stats.rituals++;
            team = teamOnTile(world, ev.x, ev.y, ev.value);
            std::snprintf(buf, sizeof(buf),
                          T("%s lance un rituel niveau %d → %d en (%d, %d)",
                            "%s starts a level %d → %d ritual at (%d, %d)"),
                          q(team).c_str(), ev.value, ev.value + 1,
                          ev.x, ev.y);
            log(buf, C_RITE);
            break;
        case FxType::IncantEnd:
            if (ev.value == 1) {
                _stats.ritualsOk++;
                log(T("Rituel réussi : les participants montent de niveau",
                      "Ritual succeeded: participants level up"),
                    C_GOOD);
            } else {
                _stats.ritualsKo++;
                log(T("Rituel raté (pierres ou joueurs manquants)",
                      "Ritual failed (missing stones or players)"), C_BAD);
            }
            break;
        case FxType::LevelUp:
            if (ev.value > bestLevel(team)) {
                _best[team] = ev.value;
                std::snprintf(buf, sizeof(buf),
                              T("%s atteint le niveau %d pour la 1re fois !",
                                "%s reaches level %d for the first time!"),
                              q(team).c_str(), ev.value);
                log(buf, Effects::levelColor(ev.value));
            }
            break;
        }
        // Les événements très fréquents remplacent le précédent du même
        // type au lieu de noyer l'historique.
        bool frequent = ev.type == FxType::Pickup || ev.type == FxType::Drop
                     || ev.type == FxType::Broadcast;
        if (frequent && !_spots.empty() && _spots.front().type == ev.type)
            _spots.pop_front();
        spot(ev, team, x, y);
    }

    if (world.gameOver && _endTime < 0) {
        _endTime = GetTime();
        log(T("Victoire de ", "Victory for ") + q(world.winnerTeam) + " !",
            TITLE);
    }
}

bool Narrator::containsMouse() const
{
    return _visible && CheckCollisionPointRec(GetMousePosition(), _rect);
}

void Narrator::drawPanel(const World &world)
{
    if (!_visible || world.width <= 0)
        return;
    float sw = static_cast<float>(GetScreenWidth());
    float sh = static_cast<float>(GetScreenHeight());
    float w  = PANEL_W;
    float x  = sw - w - 10.0f;
    float y  = 10.0f;
    _rect = {x, y, w, sh - 20.0f};
    DrawRectangleRounded(_rect, 0.03f, 8, BG);
    DrawRectangleRoundedLines(_rect, 0.03f, 8, BORDER);

    float cx = x + PAD, cw = w - 2 * PAD, cy = y + PAD;
    UiText::draw("ZAPPY", cx, cy, 26, TITLE, true);
    std::string t = T("partie en direct · ", "live game · ")
                  + formatDuration(elapsed());
    UiText::draw(t, cx + cw - UiText::width(t, 14), cy + 8, 14, MUTED);
    cy += 36;
    cy += UiText::drawWrapped(goal(), cx, cy, 13.5f, cw, TEXT, 1.3f) + 10;

    cy = drawTeams(world, cx, cy, cw) + 8;
    cy = drawCounters(cx, cy, cw) + 10;

    float footerY = y + _rect.height - 30;
    drawLog(cx, cy, cw, footerY - 8);
    DrawLine(static_cast<int>(cx), static_cast<int>(footerY - 4),
             static_cast<int>(cx + cw), static_cast<int>(footerY - 4),
             BORDER);
    UiText::draw(T("H masquer · M équipes · I infos · L English",
                   "H hide · M teams · I info · L français"),
                 cx, footerY + 2, 12.5f, MUTED);
}

float Narrator::drawTeams(const World &world, float x, float y, float w)
{
    UiText::draw(T("ÉQUIPES", "TEAMS"), x, y, 15, TITLE, true);
    const char *levels = T("niveaux 1 → 8", "levels 1 → 8");
    UiText::draw(levels, x + w - UiText::width(levels, 12),
                 y + 2, 12, MUTED);
    y += 24;
    for (const std::string &team : world.teams) {
        int perLevel[9] = {};
        int alive = 0;
        for (const auto &pair : world.players) {
            const Player &p = pair.second;
            if (!p.alive || p.team != team)
                continue;
            alive++;
            perLevel[std::clamp(p.level, 1, 8)]++;
        }
        int eggs = 0;
        for (const auto &pair : world.eggs)
            if (teamOf(world, pair.second.parentId) == team)
                eggs++;

        Color tc = teamColorFromIndex(world.teamIndex(team));
        DrawRectangleRounded({x, y + 2, 12, 12}, 0.3f, 4, tc);
        UiText::draw(team, x + 18, y, 15, TEXT, true);
        std::string info = std::to_string(alive)
            + (alive > 1 ? T(" joueurs", " players") : T(" joueur", " player"))
            + " · " + std::to_string(eggs)
            + (eggs > 1 ? T(" œufs", " eggs") : T(" œuf", " egg"));
        UiText::draw(info, x + 18, y + 18, 12, MUTED);

        // Histogramme : un bâton par niveau, hauteur = nombre de joueurs.
        float bx = x + w - 8 * 17.0f, base = y + 32, maxH = 28.0f;
        for (int lvl = 1; lvl <= 8; ++lvl) {
            float bxl = bx + (lvl - 1) * 17.0f;
            float h = perLevel[lvl] ? std::min(maxH, 6.0f + perLevel[lvl] * 4.0f)
                                    : 2.0f;
            Color c = perLevel[lvl] ? Effects::levelColor(lvl)
                                    : Color{70, 72, 85, 255};
            DrawRectangleRec({bxl, base - h, 13, h}, c);
            if (perLevel[lvl]) {
                std::string n = std::to_string(perLevel[lvl]);
                UiText::draw(n, bxl + 6.5f - UiText::width(n, 11) / 2,
                             base - h - 13, 11, TEXT);
            }
        }
        y += 40;
        if (perLevel[8] > 0) {
            char star[64];
            std::snprintf(star, sizeof(star),
                          T("★ %d / 6 joueurs au niveau 8",
                            "★ %d / 6 players at level 8"),
                          perLevel[8]);
            UiText::draw(star, x + 18, y - 4, 13, TITLE, true);
            y += 16;
        }
        y += 4;
    }
    return y;
}

float Narrator::drawCounters(float x, float y, float w)
{
    UiText::draw(T("ACTIVITÉ", "ACTIVITY"), x, y, 15, TITLE, true);
    y += 24;
    const std::pair<const char *, int> items[] = {
        {T("objets ramassés", "items picked up"), _stats.pickups},
        {T("messages radio", "radio messages"), _stats.broadcasts},
        {T("œufs pondus", "eggs laid"), _stats.eggs},
        {T("éclosions", "hatchings"), _stats.hatched},
        {T("rituels réussis", "rituals won"), _stats.ritualsOk},
        {T("rituels ratés", "rituals failed"), _stats.ritualsKo},
        {T("morts de faim", "starved"), _stats.deaths},
        {T("éjections", "ejections"), _stats.ejects},
    };
    float colW = w / 2;
    for (size_t i = 0; i < sizeof(items) / sizeof(items[0]); ++i) {
        float cx = x + (i % 2) * colW;
        float cy = y + (i / 2) * 19.0f;
        std::string n = std::to_string(items[i].second);
        UiText::draw(n, cx, cy, 14, TEXT, true);
        UiText::draw(items[i].first, cx + UiText::width(n, 14, true) + 6,
                     cy + 1, 13, MUTED);
    }
    return y + 4 * 19.0f;
}

void Narrator::drawLog(float x, float y, float w, float bottom)
{
    UiText::draw(T("JOURNAL", "EVENT LOG"), x, y, 15, TITLE, true);
    y += 24;
    for (const Entry &e : _log) {
        char ts[16];
        int sec = static_cast<int>(e.time);
        std::snprintf(ts, sizeof(ts), "%d:%02d", sec / 60, sec % 60);
        float tw = 44.0f;
        auto lines = UiText::wrap(e.text, 13, w - tw);
        float h = lines.size() * 17.0f + 4;
        if (y + h > bottom)
            break;
        UiText::draw(ts, x, y + 1, 12, MUTED);
        for (size_t i = 0; i < lines.size(); ++i)
            UiText::draw(lines[i], x + tw, y + i * 17.0f, 13, e.color);
        y += h;
    }
}

void Narrator::drawVictory(const World &world) const
{
    if (!world.gameOver)
        return;
    float sw = static_cast<float>(GetScreenWidth());
    float sh = static_cast<float>(GetScreenHeight());
    float pulse = 0.5f + 0.5f * sinf(static_cast<float>(GetTime()) * 3.0f);
    DrawRectangle(0, 0, static_cast<int>(sw), static_cast<int>(sh),
                  {0, 0, 0, 110});
    Rectangle box = {sw / 2 - 330, sh / 2 - 110, 660, 220};
    DrawRectangleRounded(box, 0.12f, 10, {14, 16, 28, 240});
    DrawRectangleRoundedLinesEx(box, 0.12f, 10, 3.0f,
                                ColorAlpha(TITLE, 0.5f + 0.5f * pulse));
    Color tc = teamColorFromIndex(world.teamIndex(world.winnerTeam));
    std::string title = T("VICTOIRE", "VICTORY");
    UiText::drawShadow(title, sw / 2 - UiText::width(title, 54, true) / 2,
                       box.y + 22, 54, TITLE, true);
    std::string name = q(world.winnerTeam);
    UiText::drawShadow(name, sw / 2 - UiText::width(name, 38, true) / 2,
                       box.y + 88, 38, tc, true);
    char buf[160];
    std::snprintf(buf, sizeof(buf),
                  T("6 joueurs au niveau 8 en %s · %d rituels réussis · %d œufs",
                    "6 players at level 8 in %s · %d rituals won · %d eggs"),
                  formatDuration(elapsed()).c_str(), _stats.ritualsOk,
                  _stats.eggs);
    UiText::draw(buf, sw / 2 - UiText::width(buf, 17) / 2, box.y + 150, 17,
                 TEXT);
    std::string hint = T("Échap pour fermer", "Esc to close");
    UiText::draw(hint, sw / 2 - UiText::width(hint, 13) / 2, box.y + 186,
                 13, MUTED);
}
