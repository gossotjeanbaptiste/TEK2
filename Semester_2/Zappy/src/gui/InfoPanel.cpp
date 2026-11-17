/*
** EPITECH PROJECT, 2026
** InfoPanel.cpp
** File description:
** On-screen panels: whole-map overview and hovered tile details
*/

#include "gui/InfoPanel.hpp"
#include "gui/MapRenderer.hpp"
#include "gui/TeamColors.hpp"
#include "gui/Lang.hpp"
#include <algorithm>
#include <cmath>
#include <map>
#include <string>
#include <vector>

namespace {

constexpr int   PANEL_X      = 10;
constexpr int   PANEL_Y      = 60;     // sous "Host/Port" et le compteur FPS
constexpr int   PANEL_W      = 290;
constexpr int   PAD          = 12;
constexpr int   LINE_H       = 20;
constexpr int   TITLE_SIZE   = 16;
constexpr int   TEXT_SIZE    = 14;
constexpr int   MAX_TILE_PLAYERS = 6;
const Color     BG           = {15, 15, 25, 225};
const Color     BORDER       = {200, 200, 210, 255};

// La police par défaut de raylib n'a pas d'accents : libellés en ASCII.
const char *RES_NAMES_FR[RESOURCE_COUNT] = {
    "Nourriture", "Linemate", "Deraumere", "Sibur",
    "Mendiane", "Phiras", "Thystame"
};
const char *RES_NAMES_EN[RESOURCE_COUNT] = {
    "Food", "Linemate", "Deraumere", "Sibur",
    "Mendiane", "Phiras", "Thystame"
};

const char *resName(int i)
{
    return Lang::english() ? RES_NAMES_EN[i] : RES_NAMES_FR[i];
}

const Color RES_COLORS[RESOURCE_COUNT] = {
    ORANGE, RED, PURPLE, BLUE, GREEN, MAGENTA, YELLOW
};

void drawFrame(Rectangle r, const char *title)
{
    DrawRectangleRec(r, BG);
    DrawRectangleLinesEx(r, 1.0f, BORDER);
    int x = static_cast<int>(r.x);
    int y = static_cast<int>(r.y);
    DrawText(title, x + PAD, y + 10, TITLE_SIZE, GOLD);
    DrawLine(x + PAD, y + 32, x + static_cast<int>(r.width) - PAD, y + 32, GRAY);
}

// Ressources sur deux colonnes ; renvoie la hauteur consommée.
int drawResourceGrid(const int counts[RESOURCE_COUNT], int x, int y)
{
    const int colW = (PANEL_W - 2 * PAD) / 2;
    for (int i = 0; i < RESOURCE_COUNT; ++i) {
        int cx = x + (i % 2) * colW;
        int cy = y + (i / 2) * LINE_H;
        DrawRectangle(cx, cy + 3, 9, 9, RES_COLORS[i]);
        std::string text = std::string(resName(i)) + " : "
                         + std::to_string(counts[i]);
        DrawText(text.c_str(), cx + 15, cy, TEXT_SIZE, WHITE);
    }
    return ((RESOURCE_COUNT + 1) / 2) * LINE_H;
}

struct TeamStats {
    int alive    = 0;
    int maxLevel = 0;
};

}  // namespace

void InfoPanel::updateHover(const World &world, const Camera3D &camera,
                            bool mouseFree)
{
    _hoverX = -1;
    _hoverY = -1;
    if (!_visible || !mouseFree || world.width <= 0 || world.height <= 0)
        return;
    // Intersection du rayon souris avec le plan du sol (y = 0, dessus des
    // dalles) : O(1) au lieu de tester chaque case.
    Ray ray = GetMouseRay(GetMousePosition(), camera);
    if (std::fabs(ray.direction.y) < 1e-6f)
        return;
    float t = -ray.position.y / ray.direction.y;
    if (t <= 0.0f)
        return;
    float px = ray.position.x + ray.direction.x * t;
    float pz = ray.position.z + ray.direction.z * t;
    int tx = static_cast<int>(std::floor(px / MapRenderer::TILE_SIZE + 0.5f));
    int ty = static_cast<int>(std::floor(pz / MapRenderer::TILE_SIZE + 0.5f));
    if (tx < 0 || ty < 0 || tx >= world.width || ty >= world.height)
        return;
    _hoverX = tx;
    _hoverY = ty;
}

bool InfoPanel::containsMouse() const
{
    if (!_visible)
        return false;
    Vector2 mouse = GetMousePosition();
    return CheckCollisionPointRec(mouse, _overview)
        || (hasHover() && CheckCollisionPointRec(mouse, _tileRect));
}

void InfoPanel::draw(const World &world)
{
    if (!_visible || world.width <= 0)
        return;
    int bottom = drawOverview(world, PANEL_X, PANEL_Y);
    if (hasHover())
        drawHoveredTile(world, PANEL_X, bottom + 10);
    else
        _tileRect = {0, 0, 0, 0};
}

int InfoPanel::drawOverview(const World &world, int x, int y)
{
    int totals[RESOURCE_COUNT] = {};
    for (const auto &row : world.tiles)
        for (const auto &tile : row)
            for (int i = 0; i < RESOURCE_COUNT; ++i)
                totals[i] += tile.resources[i];

    // Équipes connues du serveur, puis éventuelles équipes vues seulement
    // via un joueur (index -1, couleur grise).
    std::vector<std::string> order = world.teams;
    std::map<std::string, TeamStats> stats;
    int aliveTotal = 0;
    for (const auto &pair : world.players) {
        const Player &p = pair.second;
        if (!p.alive)
            continue;
        TeamStats &s = stats[p.team];
        s.alive++;
        s.maxLevel = std::max(s.maxLevel, p.level);
        aliveTotal++;
        if (std::find(order.begin(), order.end(), p.team) == order.end())
            order.push_back(p.team);
    }

    int height = 40                                  // titre
               + 2 * LINE_H                          // taille, IA/oeufs
               + 8 + LINE_H                          // titre équipes
               + static_cast<int>(std::max<size_t>(order.size(), 1)) * LINE_H
               + 8 + LINE_H                          // titre ressources
               + ((RESOURCE_COUNT + 1) / 2) * LINE_H
               + PAD;
    _overview = {static_cast<float>(x), static_cast<float>(y),
                 static_cast<float>(PANEL_W), static_cast<float>(height)};
    drawFrame(_overview, Lang::T("CARTE", "MAP"));

    int cx = x + PAD;
    int cy = y + 40;
    std::string size = Lang::T("Taille : ", "Size: ")
                     + std::to_string(world.width) + " x "
                     + std::to_string(world.height);
    DrawText(size.c_str(), cx, cy, TEXT_SIZE, SKYBLUE);
    cy += LINE_H;
    std::string counts = Lang::T("IA en vie : ", "AIs alive: ")
                       + std::to_string(aliveTotal)
                       + Lang::T("   Oeufs : ", "   Eggs: ")
                       + std::to_string(world.eggs.size());
    DrawText(counts.c_str(), cx, cy, TEXT_SIZE, WHITE);
    cy += LINE_H + 8;

    std::string teamsTitle = Lang::T("Equipes (", "Teams (")
                           + std::to_string(order.size()) + ")";
    DrawText(teamsTitle.c_str(), cx, cy, TEXT_SIZE, LIGHTGRAY);
    cy += LINE_H;
    if (order.empty()) {
        DrawText(Lang::T("aucune", "none"), cx + 10, cy, TEXT_SIZE, GRAY);
        cy += LINE_H;
    }
    for (const auto &name : order) {
        const TeamStats &s = stats[name];
        DrawRectangle(cx + 4, cy + 2, 11, 11,
                      teamColorFromIndex(world.teamIndex(name)));
        std::string label = name;
        if (label.size() > 14)
            label = label.substr(0, 13) + ".";
        DrawText(label.c_str(), cx + 22, cy, TEXT_SIZE, WHITE);
        std::string detail = std::to_string(s.alive) + Lang::T(" IA", " AI");
        if (s.alive > 0)
            detail += Lang::T("  niv max ", "  max lvl ") + std::to_string(s.maxLevel);
        DrawText(detail.c_str(), cx + 140, cy, TEXT_SIZE,
                 s.alive > 0 ? WHITE : GRAY);
        cy += LINE_H;
    }
    cy += 8;

    DrawText(Lang::T("Ressources au sol", "Resources on the ground"), cx, cy,
             TEXT_SIZE, LIGHTGRAY);
    cy += LINE_H;
    drawResourceGrid(totals, cx, cy);
    return y + height;
}

void InfoPanel::drawHoveredTile(const World &world, int x, int y)
{
    const Tile &tile = world.tiles[_hoverY][_hoverX];

    std::vector<const Player *> here;
    for (const auto &pair : world.players) {
        const Player &p = pair.second;
        if (p.alive && p.x == _hoverX && p.y == _hoverY)
            here.push_back(&p);
    }
    std::sort(here.begin(), here.end(),
              [](const Player *a, const Player *b) { return a->id < b->id; });
    int eggs = 0;
    for (const auto &pair : world.eggs)
        eggs += (pair.second.x == _hoverX && pair.second.y == _hoverY) ? 1 : 0;

    int shown = std::min(static_cast<int>(here.size()), MAX_TILE_PLAYERS);
    bool more = static_cast<int>(here.size()) > shown;
    int height = 40
               + ((RESOURCE_COUNT + 1) / 2) * LINE_H
               + 8 + LINE_H                          // "Joueurs : n"
               + shown * LINE_H + (more ? LINE_H : 0)
               + LINE_H                              // oeufs
               + (tile.isIncanting ? LINE_H : 0)
               + PAD;
    _tileRect = {static_cast<float>(x), static_cast<float>(y),
                 static_cast<float>(PANEL_W), static_cast<float>(height)};
    std::string title = Lang::T("CASE (", "TILE (") + std::to_string(_hoverX) + ", "
                      + std::to_string(_hoverY) + ")";
    drawFrame(_tileRect, title.c_str());

    int cx = x + PAD;
    int cy = y + 40;
    cy += drawResourceGrid(tile.resources, cx, cy) + 8;

    std::string players = Lang::T("Joueurs : ", "Players: ")
                        + std::to_string(here.size());
    DrawText(players.c_str(), cx, cy, TEXT_SIZE, LIGHTGRAY);
    cy += LINE_H;
    for (int i = 0; i < shown; ++i) {
        const Player *p = here[i];
        DrawRectangle(cx + 4, cy + 2, 11, 11,
                      teamColorFromIndex(world.teamIndex(p->team)));
        std::string label = "#" + std::to_string(p->id) + "  " + p->team
                          + Lang::T("  niv ", "  lvl ") + std::to_string(p->level);
        DrawText(label.c_str(), cx + 22, cy, TEXT_SIZE, WHITE);
        cy += LINE_H;
    }
    if (more) {
        std::string rest = Lang::T("... et ", "... and ")
                         + std::to_string(here.size() - shown)
                         + Lang::T(" autre(s)", " more");
        DrawText(rest.c_str(), cx + 22, cy, TEXT_SIZE, GRAY);
        cy += LINE_H;
    }
    std::string eggText = Lang::T("Oeufs : ", "Eggs: ") + std::to_string(eggs);
    DrawText(eggText.c_str(), cx, cy, TEXT_SIZE, LIGHTGRAY);
    cy += LINE_H;
    if (tile.isIncanting) {
        std::string inc = Lang::T("Incantation en cours (niv ",
                                   "Ritual in progress (lvl ")
                        + std::to_string(tile.incantationLevel) + ")";
        DrawText(inc.c_str(), cx, cy, TEXT_SIZE, MAGENTA);
    }
}
