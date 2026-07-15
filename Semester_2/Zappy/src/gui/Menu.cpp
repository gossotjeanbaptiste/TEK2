/*
** EPITECH PROJECT, 2026
** Menu.cpp
** File description:
** HUD Menu implementation
*/

#include "gui/Menu.hpp"
#include "gui/MapRenderer.hpp"

Menu::Menu() : _active(false) {}

void Menu::toggle() { _active = !_active; }
bool Menu::isActive() const { return _active; }

static Color getFixedResourceColor(int index, const Color resourceColors[])
{
    if (index == 0) return ORANGE;
    if (resourceColors != nullptr && resourceColors[index].a > 0)
        return resourceColors[index];
        
    Color fallbacks[] = {
        ORANGE,    // Food
        RED,       // Linemate
        PURPLE,    // Deraumere
        BLUE,      // Sibur
        GREEN,     // Mendiane
        MAGENTA,   // Phiras
        YELLOW     // Thystame
    };
    return fallbacks[index];
}

void Menu::draw(const World &world, const Color resourceColors[], const Camera3D &camera) const
{
    if (!_active)
        return;

    drawTileInspector(world, resourceColors, camera, 20, GetScreenHeight() - 200);

    int panelWidth = 320;
    
    drawGlobalResources(world, resourceColors, GetScreenWidth() - panelWidth - 20, 20);
    drawTeams(world, GetScreenWidth() - panelWidth - 20, 320);
}

void Menu::drawGlobalResources(const World &world, const Color resourceColors[], int posX, int posY) const
{
    int menuWidth = 320;
    int menuHeight = 280;

    DrawRectangle(posX, posY, menuWidth, menuHeight, { 15, 15, 25, 230 });
    DrawRectangleLines(posX, posY, menuWidth, menuHeight, RAYWHITE);

    DrawText("RESSOURCES GLOBALES", posX + 20, posY + 15, 16, GOLD);
    DrawLine(posX + 20, posY + 40, posX + menuWidth - 20, posY + 40, GRAY);

    std::unordered_map<Resource, int> totals;
    for (int i = 0; i < RESOURCE_COUNT; i++) totals[(Resource)i] = 0;

    for (const auto &row : world.tiles) { 
        for (const auto &tile : row) {
            for (int i = 0; i < RESOURCE_COUNT; i++) {
                totals[(Resource)i] += tile.resources[i]; 
            }
        }
    }

    std::string names[] = { "Nourriture", "Linemate", "Deraumere", "Sibur", "Mendiane", "Phiras", "Thystame" };
    int offset = 55;

    for (int i = 0; i < RESOURCE_COUNT; i++) {
        Resource r = (Resource)i;
        std::string text = names[i] + " : " + std::to_string(totals[r]);
        
        Color boxColor = getFixedResourceColor(i, resourceColors);
        
        DrawRectangle(posX + 20, posY + offset + 4, 12, 12, boxColor);
        DrawText(text.c_str(), posX + 42, posY + offset, 15, WHITE);
        offset += 30;
    }
}

void Menu::drawTeams(const World &world, int posX, int posY) const
{
    int menuWidth = 320;
    int menuHeight = 240;

    DrawRectangle(posX, posY, menuWidth, menuHeight, { 15, 15, 25, 230 });
    DrawRectangleLines(posX, posY, menuWidth, menuHeight, RAYWHITE);

    DrawText("ÉQUIPES & JOUEURS", posX + 20, posY + 15, 16, GOLD);
    DrawLine(posX + 20, posY + 40, posX + menuWidth - 20, posY + 40, GRAY);

    std::unordered_map<std::string, int> teamCounts;
    for (const auto &teamName : world.teams) {
        teamCounts[teamName] = 0;
    }

    for (const auto &pair : world.players) {
        const Player &player = pair.second;
        teamCounts[player.team]++;
    }

    int offset = 55;
    if (world.teams.empty()) {
        DrawText("Aucune équipe...", posX + 20, posY + offset, 15, LIGHTGRAY);
    } else {
        for (const auto &teamName : world.teams) {
            std::string text = teamName + " : " + std::to_string(teamCounts[teamName]) + " joueurs";
            DrawText(text.c_str(), posX + 20, posY + offset, 15, WHITE);
            offset += 25;
            if (offset > menuHeight - 30) break;
        }
    }
}

void Menu::drawTileInspector(const World &world, const Color resourceColors[], const Camera3D &camera, int posX, int posY) const
{
    Ray ray = GetMouseRay(GetMousePosition(), camera);
    const Tile *hoveredTile = nullptr;

    for (const auto &row : world.tiles) {
        for (const auto &tile : row) {
            float xPos = tile.x * MapRenderer::TILE_SIZE;
            float zPos = tile.y * MapRenderer::TILE_SIZE;
            float half = MapRenderer::TILE_SIZE / 2.0f;

            Vector3 p1 = { xPos - half, 0.0f, zPos - half };
            Vector3 p2 = { xPos + half, 0.0f, zPos - half };
            Vector3 p3 = { xPos + half, 0.0f, zPos + half };
            Vector3 p4 = { xPos - half, 0.0f, zPos + half };

            RayCollision col = GetRayCollisionQuad(ray, p1, p2, p3, p4);
            if (col.hit) {
                hoveredTile = &tile;
                break;
            }
        }
        if (hoveredTile) break;
    }

    int width = 260;
    int height = 180;
    DrawRectangle(posX, posY, width, height, { 10, 10, 15, 240 });
    DrawRectangleLines(posX, posY, width, height, GOLD);

    DrawText("INSPECTEUR DE CASE", posX + 15, posY + 12, 14, GOLD);
    
    if (!hoveredTile) {
        DrawText("Survolez une case", posX + 15, posY + 50, 15, LIGHTGRAY);
        DrawText("avec la souris...", posX + 15, posY + 75, 15, LIGHTGRAY);
        return;
    }

    std::string coord = "Case X: " + std::to_string(hoveredTile->x) + " Y: " + std::to_string(hoveredTile->y);
    DrawText(coord.c_str(), posX + 15, posY + 35, 14, SKYBLUE);
    DrawLine(posX + 15, posY + 52, posX + width - 15, posY + 52, GRAY);

    std::string names[] = { "Nourriture", "Linemate", "Deraumere", "Sibur", "Mendiane", "Phiras", "Thystame" };
    int itemOffset = 60;
    int columnX = 0;

    for (int i = 0; i < RESOURCE_COUNT; i++) {
        int textX = posX + 15 + (columnX * 120);
        int textY = posY + itemOffset;

        std::string resText = names[i].substr(0, 4) + ". : " + std::to_string(hoveredTile->resources[i]);
        
        Color boxColor = getFixedResourceColor(i, resourceColors);

        DrawRectangle(textX, textY + 3, 8, 8, boxColor);
        DrawText(resText.c_str(), textX + 14, textY, 13, WHITE);

        columnX++;
        if (columnX > 1) {
            columnX = 0;
            itemOffset += 22;
        }
    }
    
    if (hoveredTile->isIncanting) {
        DrawText("INCANTATION EN COURS !", posX + 15, posY + 155, 12, RED);
    }
}