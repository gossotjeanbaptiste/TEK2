/*
** EPITECH PROJECT, 2026
** MapRenderer.cpp
*/

#include "gui/MapRenderer.hpp"
#include <raylib.h>
#include <cmath>

void MapRenderer::draw(const World &world) const
{
    if (world.width == 0) return;

    // 1. DESSIN DU SOL / GRILLE
    for (int y = 0; y < world.height; ++y) {
        for (int x = 0; x < world.width; ++x) {
            Vector3 tileCenter = { x * TILE_SIZE, -0.5f, y * TILE_SIZE };
            float sz = TILE_SIZE - 0.05f;
            
            const Tile &tile = world.tiles[y][x];
            Color floorColor = DARKGREEN;
            if (tile.isIncanting) {
                floorColor = PURPLE; // Devient violet si incantation en cours
            }

            DrawCube(tileCenter, sz, 1.0f, sz, floorColor);
            DrawCubeWires(tileCenter, sz, 1.0f, sz, tile.isIncanting ? MAGENTA : GREEN);

            // 2. DISPERSION ET DESSIN DES RESSOURCES SUR LA TUILE
            int resourceIndex = 0;
            for (int r = 0; r < RESOURCE_COUNT; ++r) {
                int count = tile.resources[r];
                for (int i = 0; i < count; ++i) {
                    // Petite formule pour espacer les ressources en cercle sur la case
                    float angle = resourceIndex * (360.0f / 8.0f) * DEG2RAD;
                    float dist = 0.4f + (resourceIndex * 0.03f);
                    
                    Vector3 resPos = {
                        tileCenter.x + cosf(angle) * dist,
                        0.1f, // Posé juste au-dessus du sol
                        tileCenter.z + sinf(angle) * dist
                    };

                    // Appel du Placeholder !
                    _placeholder.drawResource(static_cast<Resource>(r), resPos);
                    resourceIndex++;
                }
            }
        }
    }

    // 3. DESSIN DES ŒUFS
    for (const auto &pair : world.eggs) {
        const Egg &egg = pair.second;
        Vector3 eggPos = { egg.x * TILE_SIZE, 0.1f, egg.y * TILE_SIZE };
        _placeholder.drawEgg(eggPos);
    }

    // 4. DESSIN DES JOUEURS
    for (const auto &pair : world.players) {
        const Player &p = pair.second;
        if (!p.alive) continue;

        // Les pieds du modèle sont à l'origine : on pose le joueur sur la case
        Vector3 playerPos = { p.x * TILE_SIZE, 0.0f, p.y * TILE_SIZE };
        
        // Attribuer une couleur par équipe
        int tIdx = world.teamIndex(p.team);
        Color teamColor = (tIdx == 0) ? RED : (tIdx == 1) ? BLUE : (tIdx == 2) ? GOLD : ORANGE;

        // Appel du Placeholder !
        _placeholder.drawPlayer(playerPos, static_cast<int>(p.orientation), teamColor);
    }
}