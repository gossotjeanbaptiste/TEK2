/*
** EPITECH PROJECT, 2026
** Menu.cpp
** File description:
** HUD Menu implementation
*/

#include "gui/Menu.hpp"
#include "gui/MapRenderer.hpp"
#include "gui/TeamColors.hpp"
#include "gui/Lang.hpp"
#include <algorithm>
#include <vector>

Menu::Menu() : _active(false) {}

void Menu::toggle() { _active = !_active; }
bool Menu::isActive() const { return _active; }

bool Menu::isMouseCaptured() const
{
    if (!_active)
        return false;
    return CheckCollisionPointRec(GetMousePosition(), _teamsRect);
}

void Menu::draw(const World &world)
{
    if (!_active)
        return;
    // Ressources globales et case survolée : voir InfoPanel (toujours
    // visible en haut à gauche). Le menu M ne garde que les équipes.
    int panelWidth = 320;
    drawTeams(world, GetScreenWidth() - panelWidth - _rightMargin, 20);
}

void Menu::drawTeams(const World &world, int posX, int posY)
{
    int menuWidth = 320;

    // Joueurs regroupés par équipe, triés par id pour un ordre stable.
    std::unordered_map<std::string, std::vector<const Player *>> byTeam;
    for (const auto &pair : world.players)
        byTeam[pair.second.team].push_back(&pair.second);
    for (auto &pair : byTeam)
        std::sort(pair.second.begin(), pair.second.end(),
                  [](const Player *a, const Player *b) { return a->id < b->id; });

    // Hauteur adaptée au contenu (listes dépliées comprises).
    int menuHeight = 67;
    for (const auto &teamName : world.teams) {
        menuHeight += 25;
        if (_expandedTeams.count(teamName))
            menuHeight += static_cast<int>(byTeam[teamName].size()) * 22 + 6;
    }
    if (world.teams.empty())
        menuHeight += 25;
    int maxHeight = GetScreenHeight() - posY - 20;
    if (menuHeight > maxHeight)
        menuHeight = maxHeight;

    _teamsRect = {static_cast<float>(posX), static_cast<float>(posY),
                  static_cast<float>(menuWidth), static_cast<float>(menuHeight)};

    DrawRectangle(posX, posY, menuWidth, menuHeight, { 15, 15, 25, 230 });
    DrawRectangleLines(posX, posY, menuWidth, menuHeight, RAYWHITE);

    DrawText(Lang::T("ÉQUIPES & JOUEURS", "TEAMS & PLAYERS"), posX + 20,
             posY + 15, 16, GOLD);
    DrawLine(posX + 20, posY + 40, posX + menuWidth - 20, posY + 40, GRAY);

    Vector2 mouse   = GetMousePosition();
    bool    clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    int offset = 55;
    if (world.teams.empty()) {
        DrawText(Lang::T("Aucune équipe...", "No team..."), posX + 20,
                 posY + offset, 15, LIGHTGRAY);
        return;
    }
    for (const auto &teamName : world.teams) {
        if (offset > menuHeight - 25)
            break;
        const auto &members  = byTeam[teamName];
        bool        expanded = _expandedTeams.count(teamName) > 0;

        // Ligne d'équipe : cliquer déplie/replie la liste de ses joueurs.
        Rectangle row = {static_cast<float>(posX + 12),
                         static_cast<float>(posY + offset - 2),
                         static_cast<float>(menuWidth - 24), 22.0f};
        bool hovered = CheckCollisionPointRec(mouse, row);
        if (hovered)
            DrawRectangleRec(row, {255, 255, 255, 25});
        if (hovered && clicked) {
            if (expanded)
                _expandedTeams.erase(teamName);
            else
                _expandedTeams.insert(teamName);
            expanded = !expanded;
        }

        int alive = 0;
        for (const Player *p : members)
            alive += p->alive ? 1 : 0;

        Color teamColor = teamColorFromIndex(world.teamIndex(teamName));
        DrawText(expanded ? "v" : ">", posX + 20, posY + offset, 15, LIGHTGRAY);
        DrawRectangle(posX + 34, posY + offset + 2, 12, 12, teamColor);
        std::string text = teamName + " : " + std::to_string(alive)
                         + Lang::T(" joueurs", " players");
        DrawText(text.c_str(), posX + 54, posY + offset, 15, WHITE);
        offset += 25;

        if (!expanded)
            continue;

        // Lignes des joueurs : cliquer (dé)sélectionne le joueur à suivre.
        for (const Player *p : members) {
            if (offset > menuHeight - 20)
                break;
            Rectangle prow = {static_cast<float>(posX + 30),
                              static_cast<float>(posY + offset - 2),
                              static_cast<float>(menuWidth - 46), 20.0f};
            bool ph      = CheckCollisionPointRec(mouse, prow);
            bool watched = (_watchId == p->id);
            if (watched)
                DrawRectangleRec(prow, {255, 203, 0, 60});
            else if (ph)
                DrawRectangleRec(prow, {255, 255, 255, 25});
            if (ph && clicked) {
                _watchId = watched ? -1 : p->id;
                watched  = !watched;
            }

            std::string label = "#" + std::to_string(p->id)
                              + Lang::T("  niv ", "  lvl ") + std::to_string(p->level);
            if (!p->alive)
                label += Lang::T("  (mort)", "  (dead)");
            DrawText(label.c_str(), posX + 40, posY + offset, 14,
                     p->alive ? WHITE : GRAY);
            if (watched)
                DrawText(Lang::T("[SUIVI]", "[WATCHING]"), posX + menuWidth - 86,
                         posY + offset, 13, GOLD);
            else if (ph)
                DrawText(Lang::T("[suivre]", "[watch]"), posX + menuWidth - 86,
                         posY + offset, 13, LIGHTGRAY);
            offset += 22;
        }
        offset += 6;
    }
}
