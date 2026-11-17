/*
** EPITECH PROJECT, 2026
** MapRenderer.cpp
*/

#include "gui/MapRenderer.hpp"
#include "gui/TeamColors.hpp"
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <cmath>
#include <unordered_map>
#include <utility>


namespace {
    // Une case = un pavé (dessus + 4 côtés ; le dessous n'est jamais vu),
    // 2 triangles par face, sans indices.
    constexpr int FLOOR_VERTS_PER_TILE = 5 * 6;
    constexpr float TILE_GAP = 0.05f;
}

MapRenderer::~MapRenderer()
{
    if (_floorBuilt)
        UnloadMesh(_floor);
    if (_floorMatLoaded)
        UnloadMaterial(_floorMat);
}

void MapRenderer::buildFloor(int width, int height)
{
    if (_floorBuilt) {
        UnloadMesh(_floor);
        _floor      = {};
        _floorBuilt = false;
    }
    if (!_floorMatLoaded) {
        _floorMat       = LoadMaterialDefault();
        _floorMatLoaded = true;
    }

    int tiles = width * height;
    Mesh m = {};
    m.vertexCount   = tiles * FLOOR_VERTS_PER_TILE;
    m.triangleCount = m.vertexCount / 3;
    m.vertices  = static_cast<float *>(MemAlloc(m.vertexCount * 3 * sizeof(float)));
    m.texcoords = static_cast<float *>(MemAlloc(m.vertexCount * 2 * sizeof(float)));
    m.colors    = static_cast<unsigned char *>(MemAlloc(m.vertexCount * 4));

    int v = 0;
    auto put = [&](Vector3 p) {
        m.vertices[v * 3]     = p.x;
        m.vertices[v * 3 + 1] = p.y;
        m.vertices[v * 3 + 2] = p.z;
        ++v;
    };
    // Quadrilatère abcd tourné vers n : sens anti-horaire vu de l'extérieur
    // (faces avant d'OpenGL, comme DrawCube), sinon il serait éliminé.
    auto quad = [&](Vector3 a, Vector3 b, Vector3 c, Vector3 d, Vector3 n) {
        Vector3 cross = Vector3CrossProduct(Vector3Subtract(b, a),
                                            Vector3Subtract(c, a));
        if (Vector3DotProduct(cross, n) < 0.0f)
            std::swap(b, d);
        put(a); put(b); put(c);
        put(a); put(c); put(d);
    };

    // Mêmes pavés que l'ancien DrawCube par case : centre y = -0.5,
    // hauteur 1, côté TILE_SIZE - TILE_GAP.
    float half = (TILE_SIZE - TILE_GAP) * 0.5f;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float x0 = x * TILE_SIZE - half, x1 = x * TILE_SIZE + half;
            float z0 = y * TILE_SIZE - half, z1 = y * TILE_SIZE + half;
            float y0 = -1.0f, y1 = 0.0f;
            quad({x0, y1, z0}, {x0, y1, z1}, {x1, y1, z1}, {x1, y1, z0}, {0, 1, 0});
            quad({x1, y0, z0}, {x1, y1, z0}, {x1, y1, z1}, {x1, y0, z1}, {1, 0, 0});
            quad({x0, y0, z0}, {x0, y0, z1}, {x0, y1, z1}, {x0, y1, z0}, {-1, 0, 0});
            quad({x0, y0, z1}, {x1, y0, z1}, {x1, y1, z1}, {x0, y1, z1}, {0, 0, 1});
            quad({x0, y0, z0}, {x0, y1, z0}, {x1, y1, z0}, {x1, y0, z0}, {0, 0, -1});
        }
    }
    for (int i = 0; i < m.vertexCount; ++i) {
        m.colors[i * 4]     = DARKGREEN.r;
        m.colors[i * 4 + 1] = DARKGREEN.g;
        m.colors[i * 4 + 2] = DARKGREEN.b;
        m.colors[i * 4 + 3] = DARKGREEN.a;
    }
    UploadMesh(&m, false);

    _floor      = m;
    _floorBuilt = true;
    _floorW     = width;
    _floorH     = height;
    _floorIncanting.assign(tiles, 0);
}

void MapRenderer::syncFloorColors(const World &world)
{
    // Seules les cases dont l'état d'incantation change sont renvoyées au GPU.
    constexpr int bytes = FLOOR_VERTS_PER_TILE * 4;
    for (int y = 0; y < _floorH; ++y) {
        for (int x = 0; x < _floorW; ++x) {
            int  i   = y * _floorW + x;
            bool inc = world.tiles[y][x].isIncanting;
            if (static_cast<bool>(_floorIncanting[i]) == inc)
                continue;
            _floorIncanting[i] = inc;
            Color c = inc ? PURPLE : DARKGREEN; // violet pendant une incantation
            unsigned char *dst = _floor.colors + i * bytes;
            for (int k = 0; k < FLOOR_VERTS_PER_TILE; ++k) {
                dst[k * 4]     = c.r;
                dst[k * 4 + 1] = c.g;
                dst[k * 4 + 2] = c.b;
                dst[k * 4 + 3] = c.a;
            }
            UpdateMeshBuffer(_floor, RL_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR,
                             dst, bytes, i * bytes);
        }
    }
}

void MapRenderer::drawGridLines(const World &world) const
{
    // Contours des cases en un seul lot de lignes (avant : un DrawCubeWires
    // par case, qui coupait le lot raylib à chaque case). Les arêtes du bas
    // restent toujours cachées sous le sol du décor : inutile de les tracer.
    float half = (TILE_SIZE - TILE_GAP) * 0.5f;
    rlBegin(RL_LINES);
    for (int y = 0; y < world.height; ++y) {
        for (int x = 0; x < world.width; ++x) {
            Color c = world.tiles[y][x].isIncanting ? MAGENTA : GREEN;
            rlColor4ub(c.r, c.g, c.b, c.a);
            float x0 = x * TILE_SIZE - half, x1 = x * TILE_SIZE + half;
            float z0 = y * TILE_SIZE - half, z1 = y * TILE_SIZE + half;
            const float cx[4] = {x0, x1, x1, x0};
            const float cz[4] = {z0, z0, z1, z1};
            for (int k = 0; k < 4; ++k) {
                int n = (k + 1) % 4;
                rlVertex3f(cx[k], 0.0f, cz[k]);
                rlVertex3f(cx[n], 0.0f, cz[n]);
                rlVertex3f(cx[k], -1.0f, cz[k]);
                rlVertex3f(cx[k], 0.0f, cz[k]);
            }
        }
    }
    rlEnd();
}

void MapRenderer::draw(const World &world, int hoverX, int hoverY)
{
    if (world.width <= 0 || world.height <= 0) return;

    // 0. DÉCOR AUTOUR DE LA CARTE (généré une fois, purement cosmétique)
    _decor.ensureBuilt(world.width, world.height, TILE_SIZE);
    _decor.draw();

    // 1. RESSOURCES : regroupées par type, puis dessinées par instanciation.
    //    Elles passent AVANT le sol : certains cristaux sont semi-transparents
    //    et se fondaient jusqu'ici avec le fond de la scène (le sol de
    //    l'ancien code n'était envoyé au GPU qu'après elles). Dessinées après
    //    le sol, elles prendraient sa teinte verte.
    for (auto &list : _resourcePositions)
        list.clear();
    for (int y = 0; y < world.height; ++y) {
        for (int x = 0; x < world.width; ++x) {
            const Tile &tile = world.tiles[y][x];
            int resourceIndex = 0;
            for (int r = 0; r < RESOURCE_COUNT; ++r) {
                for (int i = 0; i < tile.resources[r]; ++i) {
                    // Petite formule pour espacer les ressources en cercle sur la case
                    float angle = resourceIndex * (360.0f / 8.0f) * DEG2RAD;
                    float dist  = 0.4f + (resourceIndex * 0.03f);
                    _resourcePositions[r].push_back({
                        x * TILE_SIZE + cosf(angle) * dist,
                        0.1f, // Posé juste au-dessus du sol
                        y * TILE_SIZE + sinf(angle) * dist});
                    resourceIndex++;
                }
            }
        }
    }
    for (int r = 0; r < RESOURCE_COUNT; ++r)
        _placeholder.drawResources(static_cast<Resource>(r), _resourcePositions[r]);

    // 2. SOL / GRILLE : un mesh statique pour toutes les cases (un seul
    //    appel de dessin), couleurs mises à jour lors des incantations.
    if (!_floorBuilt || _floorW != world.width || _floorH != world.height)
        buildFloor(world.width, world.height);
    syncFloorColors(world);
    DrawMesh(_floor, _floorMat, MatrixIdentity());
    drawGridLines(world);

    // Case survolée (panneau CASE) : cadre jaune légèrement surélevé.
    if (hoverX >= 0 && hoverY >= 0 && hoverX < world.width && hoverY < world.height) {
        Vector3 c = { hoverX * TILE_SIZE, 0.02f, hoverY * TILE_SIZE };
        DrawCubeWires(c, TILE_SIZE, 0.04f, TILE_SIZE, YELLOW);
        DrawCube(c, TILE_SIZE - 0.1f, 0.01f, TILE_SIZE - 0.1f, {255, 255, 0, 50});
    }

    // 3. DESSIN DES ŒUFS
    _eggPositions.clear();
    for (const auto &pair : world.eggs) {
        const Egg &egg = pair.second;
        _eggPositions.push_back({egg.x * TILE_SIZE, 0.1f, egg.y * TILE_SIZE});
    }
    _placeholder.drawEggs(_eggPositions);

    // 4. DESSIN DES JOUEURS
    // La "reine" de chaque équipe (le doyen : plus petit id vivant, ce qui
    // correspond au premier client connecté, celui que l'IA élit reine)
    // porte la couronne à la place de la gemme d'équipe.
    std::unordered_map<int, int> queenByTeam;
    for (const auto &pair : world.players) {
        const Player &p = pair.second;
        if (!p.alive)
            continue;
        int team = world.teamIndex(p.team);
        auto it  = queenByTeam.find(team);
        if (it == queenByTeam.end() || p.id < it->second)
            queenByTeam[team] = p.id;
    }

    for (const auto &pair : world.players) {
        const Player &p = pair.second;

        // Position interpolée par le GUI : le joueur glisse vers sa nouvelle
        // case (animation Walk) au lieu d'être téléporté dessus.
        Vector2 visTile = _placeholder.getVisualTilePos(p.id, p.x, p.y);
        Vector3 playerPos = { visTile.x * TILE_SIZE, 0.3f, visTile.y * TILE_SIZE };

        // Attribuer une couleur par équipe (palette partagée avec le HUD)
        int   teamIdx   = world.teamIndex(p.team);
        Color teamColor = teamColorFromIndex(teamIdx);

        auto queenIt = queenByTeam.find(teamIdx);
        bool isQueen = (queenIt != queenByTeam.end() && queenIt->second == p.id);

        // Appel du Placeholder ! (le joueur mort reste affiché le temps de son animation)
        _placeholder.drawPlayer(p.id, playerPos, static_cast<int>(p.orientation),
                                p.level, teamColor, isQueen);
    }
    _placeholder.flushGems();

    // 5. EFFETS VISUELS (translucides, donc en dernier)
    _effects.draw(world, _placeholder, TILE_SIZE);
}
