/*
** EPITECH PROJECT, 2026
** Decor.cpp
** File description:
** Procedural decorative ring around the game map (Kenney Survival Kit)
*/

#include "gui/Decor.hpp"
#include "gui/GpuDraw.hpp"
#include <cmath>
#include <iostream>
#include <random>

static const char *DECOR_ROOT = "assets/models/";

Decor::~Decor()
{
    for (auto &m : _models) {
        if (m.loaded)
            UnloadModel(m.model);
    }
    if (_oceanLoaded)
        UnloadModel(_ocean);
}

int Decor::loadOne(const std::string &relPath, float targetHeights)
{
    std::string path = DECOR_ROOT + relPath;

    if (!FileExists(path.c_str())) {
        std::clog << "[Asset Warning] Decor not found: " << path << "\n";
        return -1;
    }

    DecorModel dm;
    dm.model = LoadModel(path.c_str());

    // Normalise le modèle pour que sa plus grande dimension fasse
    // targetHeights fois la hauteur du personnage, et retient le bas du
    // modèle pour le poser sur le sol quelle que soit l'origine de l'export.
    float targetSize = targetHeights * _charHeight;
    BoundingBox b = GetModelBoundingBox(dm.model);
    float largest = fmaxf(b.max.x - b.min.x,
                    fmaxf(b.max.y - b.min.y, b.max.z - b.min.z));
    dm.baseScale = (largest > 0.0f) ? targetSize / largest : 1.0f;
    dm.minY      = b.min.y;
    dm.loaded    = true;

    _models.push_back(dm);
    return static_cast<int>(_models.size()) - 1;
}

void Decor::loadAll(float characterHeight)
{
    if (characterHeight > 0.0f)
        _charHeight = characterHeight;

    std::clog << "[Asset] Loading decor (Survival Kit)...\n";

    struct ScatterDef {
        const char *path;
        float       size;
        int         weight;
    };
    // Tailles en multiples de la hauteur du personnage (un arbre fait ~2
    // personnages, une maison ~2, le château ~4.5...).
    // "Tree Fall" = arbre d'automne (orange), pas un tronc couché.
    static const ScatterDef SCATTER[] = {
        {"Survival_Kit/Tree/tree.obj",                     1.9f, 20},
        {"Survival_Kit/Tree Large/treeLarge.obj",          2.5f, 10},
        {"Survival_Kit/Tree Fall/treeFall.obj",            1.9f,  7},
        {"Survival_Kit/Tree Fall Large/treeFallLarge.obj", 2.5f,  4},
        {"Survival_Kit/Grass/grass.obj",                   0.30f, 16},
        {"Survival_Kit/Grass/grassLarge.obj",              0.42f, 10},
        {"Survival_Kit/Rock/rockA.obj",                    0.50f,  4},
        {"Survival_Kit/Rock/rockB.obj",                    0.50f,  4},
        {"Survival_Kit/Rock/rockC.obj",                    0.50f,  4},
        {"Survival_Kit/Rock Flat/rockFlat.obj",            0.60f,  4},
        {"Survival_Kit/Rock Flat Grass/rockFlatGrass.obj", 0.60f,  5},
        {"Survival_Kit/Twig/resourceWood.obj",             0.55f,  3},
        {"Survival_Kit/Resource Stone/resourceStone.obj",  0.45f,  3},
    };

    for (const auto &def : SCATTER) {
        int idx = loadOne(def.path, def.size);
        if (idx >= 0)
            _scatter.push_back({idx, def.weight});
    }

    _tent      = loadOne("Survival_Kit/Tent/tentClosed.obj",     1.20f);
    _campfire  = loadOne("Survival_Kit/Campfire/campfire.obj",   0.55f);
    _barrel    = loadOne("Survival_Kit/Barrel/barrel.obj",       0.50f);
    _chest     = loadOne("Survival_Kit/Chest/chest.obj",         0.50f);
    _signpost  = loadOne("Survival_Kit/Signpost/signpost.obj",   0.85f);
    _workbench = loadOne("Survival_Kit/Workbench/workbench.obj", 0.65f);
    _bedroll   = loadOne("Survival_Kit/Bedroll/bedroll.obj",     0.80f);

    // Village médiéval et château (packs FBX convertis en GLB via Blender,
    // voir assets/models/Decor/).
    _house1     = loadOne("Decor/House_1.glb",    2.0f);
    _house2     = loadOne("Decor/House_2.glb",    2.0f);
    _house4     = loadOne("Decor/House_4.glb",    2.2f);
    _inn        = loadOne("Decor/Inn.glb",        2.6f);
    _well       = loadOne("Decor/Well.glb",       0.7f);
    _mill       = loadOne("Decor/Mill.glb",       3.2f);
    _castle     = loadOne("Decor/Castle.glb",     4.5f);
    _watchTower = loadOne("Decor/WatchTower.glb", 2.6f);

    // Océan entourant l'île : chargé brut, mis à l'échelle au dessin (non
    // uniforme : la surface couvre la zone, la hauteur des vagues est capée).
    std::string oceanPath = std::string(DECOR_ROOT) + "Ocean.glb";
    if (FileExists(oceanPath.c_str())) {
        _ocean       = LoadModel(oceanPath.c_str());
        _oceanBounds = GetModelBoundingBox(_ocean);
        _oceanLoaded = true;
    } else {
        std::clog << "[Asset Warning] Decor not found: " << oceanPath << "\n";
    }
}

void Decor::addInstance(int modelIndex, float tileX, float tileY, float yawDeg, float scaleMul)
{
    if (modelIndex < 0)
        return;

    const DecorModel &m = _models[modelIndex];
    float scale = m.baseScale * scaleMul;

    Instance inst;
    inst.modelIndex = modelIndex;
    inst.position   = {tileX * _tileSize, GROUND_Y - m.minY * scale, tileY * _tileSize};
    inst.yawDeg     = yawDeg;
    inst.scale      = scale;
    _instances.push_back(inst);
}

void Decor::ensureBuilt(int mapWidth, int mapHeight, float tileSize)
{
    if (mapWidth <= 0 || mapHeight <= 0)
        return;
    if (mapWidth == _builtW && mapHeight == _builtH)
        return;

    _builtW   = mapWidth;
    _builtH   = mapHeight;
    _tileSize = tileSize;
    _instances.clear();

    // Graine dérivée de la taille de la carte : le décor est identique d'une
    // frame et d'un lancement à l'autre pour une même carte.
    std::mt19937 rng(0x5A50F00Du
        ^ (static_cast<unsigned>(mapWidth) * 73856093u)
        ^ (static_cast<unsigned>(mapHeight) * 19349663u));
    std::uniform_real_distribution<float> u01(0.0f, 1.0f);

    // Petit campement sur le bord nord de la carte.
    float campX = (mapWidth - 1) / 2.0f;
    addInstance(_tent,      campX - 1.3f, -2.9f, 150.0f, 1.0f);
    addInstance(_campfire,  campX,        -2.0f,  30.0f, 1.0f);
    addInstance(_barrel,    campX + 1.1f, -2.7f,   0.0f, 1.0f);
    addInstance(_chest,     campX + 1.8f, -2.2f, 200.0f, 1.0f);
    addInstance(_signpost,  campX - 2.3f, -1.5f, 170.0f, 1.0f);
    addInstance(_workbench, campX + 2.4f, -3.1f, 220.0f, 1.0f);
    addInstance(_bedroll,   campX - 2.2f, -2.5f, 100.0f, 1.0f);

    // Village médiéval sur le bord sud.
    float villX = (mapWidth - 1) / 2.0f;
    float south = static_cast<float>(mapHeight);
    addInstance(_mill,   villX - 4.6f, south + 2.4f, 200.0f, 1.0f);
    addInstance(_inn,    villX - 2.6f, south + 1.9f, 180.0f, 1.0f);
    addInstance(_house1, villX - 0.5f, south + 1.3f, 170.0f, 1.0f);
    addInstance(_well,   villX + 0.8f, south + 0.7f,  40.0f, 1.0f);
    addInstance(_house2, villX + 1.6f, south + 1.8f, 190.0f, 1.0f);
    addInstance(_house4, villX + 3.4f, south + 1.3f, 160.0f, 1.0f);

    // Château sur le bord est, flanqué de deux tours de garde.
    float east  = static_cast<float>(mapWidth);
    float castY = (mapHeight - 1) / 2.0f;
    addInstance(_castle,     east + 2.6f, castY,        270.0f, 1.0f);
    addInstance(_watchTower, east + 1.2f, castY - 2.8f, 270.0f, 1.0f);
    addInstance(_watchTower, east + 1.2f, castY + 2.8f, 270.0f, 1.0f);

    int totalWeight = 0;
    for (const auto &s : _scatter)
        totalWeight += s.second;
    if (totalWeight == 0) {
        rebuildBatches();
        return;
    }

    for (int cy = -RING_DEPTH; cy < mapHeight + RING_DEPTH; ++cy) {
        for (int cx = -RING_DEPTH; cx < mapWidth + RING_DEPTH; ++cx) {
            bool insideMap = (cx >= 0 && cx < mapWidth && cy >= 0 && cy < mapHeight);
            if (insideMap)
                continue;
            // Laisse dégagées les zones du campement, du village et du château.
            if (fabsf(static_cast<float>(cx) - campX) < 3.5f && cy >= -4 && cy <= -1)
                continue;
            if (fabsf(static_cast<float>(cx) - villX) < 6.0f
                && cy >= mapHeight && cy <= mapHeight + 4)
                continue;
            if (fabsf(static_cast<float>(cy) - castY) < 4.5f
                && cx >= mapWidth && cx <= mapWidth + 5)
                continue;
            // Couloir de l'accès à la plage (chemin de sable, bord ouest).
            if (cx < 0 && fabsf(static_cast<float>(cy) - castY) < 1.8f)
                continue;
            if (u01(rng) > PROP_DENSITY)
                continue;

            // Tirage pondéré du modèle.
            int pick = static_cast<int>(u01(rng) * static_cast<float>(totalWeight));
            int modelIndex = _scatter.back().first;
            for (const auto &s : _scatter) {
                pick -= s.second;
                if (pick < 0) {
                    modelIndex = s.first;
                    break;
                }
            }

            float px = static_cast<float>(cx) + (u01(rng) - 0.5f) * 0.7f;
            float py = static_cast<float>(cy) + (u01(rng) - 0.5f) * 0.7f;
            addInstance(modelIndex, px, py, u01(rng) * 360.0f, 0.8f + u01(rng) * 0.4f);
        }
    }
    rebuildBatches();
}

void Decor::rebuildBatches()
{
    // Le décor est fixe : ses matrices sont calculées une fois, rangées par
    // modèle, pour dessiner toutes les copies d'un modèle en un appel par
    // mesh (avant : un DrawModelEx par arbre, rocher, touffe d'herbe...).
    _batches.assign(_models.size(), {});
    for (const auto &inst : _instances) {
        const DecorModel &m = _models[inst.modelIndex];
        Vector3 scale = {inst.scale, inst.scale, inst.scale};
        _batches[inst.modelIndex].push_back(
            GpuDraw::modelTransform(m.model, inst.position, inst.yawDeg, scale));
    }
}

void Decor::draw() const
{
    if (_builtW == 0)
        return;

    float ts   = _tileSize;
    float minX = (-RING_DEPTH - 0.5f) * ts;
    float maxX = (_builtW - 1 + RING_DEPTH + 0.5f) * ts;
    float minZ = (-RING_DEPTH - 0.5f) * ts;
    float maxZ = (_builtH - 1 + RING_DEPTH + 0.5f) * ts;

    // Sol du décor, légèrement sous le dessus des cases de jeu ; il affleure
    // aussi dans les interstices de la grille (effet "terre" entre les cases).
    Vector3 center = {(minX + maxX) * 0.5f, GROUND_Y - 0.5f, (minZ + maxZ) * 0.5f};
    DrawCube(center, maxX - minX, 1.0f, maxZ - minZ, Color{58, 92, 48, 255});

    drawBeach(minX, maxX, minZ, maxZ);

    for (size_t i = 0; i < _batches.size(); ++i) {
        const std::vector<Matrix> &batch = _batches[i];
        GpuDraw::drawModelInstanced(_models[i].model, batch.data(),
                                    static_cast<int>(batch.size()), WHITE);
    }

    // Océan en dernier : s'il a un matériau semi-transparent, il se fond
    // correctement avec ce qui est déjà dessiné derrière/dessous.
    drawOcean(minX, maxX, minZ, maxZ);
}

void Decor::drawBeach(float minX, float maxX, float minZ, float maxZ) const
{
    const Color sand = {228, 211, 160, 255};
    float ts    = _tileSize;
    float bMinX = minX - BEACH_DEPTH * ts;
    float bMaxX = maxX + BEACH_DEPTH * ts;
    float bMinZ = minZ - BEACH_DEPTH * ts;
    float bMaxZ = maxZ + BEACH_DEPTH * ts;
    float cy    = BEACH_TOP_Y - 0.5f;

    // Plage tout autour de l'île : bandes nord/sud, puis bandes ouest/est
    // qui couvrent aussi les quatre coins.
    DrawCube({(minX + maxX) * 0.5f, cy, (bMinZ + minZ) * 0.5f},
             maxX - minX, 1.0f, minZ - bMinZ, sand);
    DrawCube({(minX + maxX) * 0.5f, cy, (maxZ + bMaxZ) * 0.5f},
             maxX - minX, 1.0f, bMaxZ - maxZ, sand);
    DrawCube({(bMinX + minX) * 0.5f, cy, (bMinZ + bMaxZ) * 0.5f},
             minX - bMinX, 1.0f, bMaxZ - bMinZ, sand);
    DrawCube({(maxX + bMaxX) * 0.5f, cy, (bMinZ + bMaxZ) * 0.5f},
             bMaxX - maxX, 1.0f, bMaxZ - bMinZ, sand);

    // Petit accès à la plage (bord ouest) : un chemin de sable qui traverse
    // l'anneau de décor, du bord de la carte de jeu jusqu'au sable.
    float pathZ    = ((_builtH - 1) / 2.0f) * ts;
    float pathEndX = -0.5f * ts;
    DrawCube({(minX + pathEndX) * 0.5f, -0.09f, pathZ},
             pathEndX - minX, 0.12f, 2.2f * ts, sand);
}

void Decor::drawOcean(float minX, float maxX, float minZ, float maxZ) const
{
    if (!_oceanLoaded)
        return;

    float ts    = _tileSize;
    float oMinX = minX - (BEACH_DEPTH + OCEAN_EXTENT) * ts;
    float oMaxX = maxX + (BEACH_DEPTH + OCEAN_EXTENT) * ts;
    float oMinZ = minZ - (BEACH_DEPTH + OCEAN_EXTENT) * ts;
    float oMaxZ = maxZ + (BEACH_DEPTH + OCEAN_EXTENT) * ts;

    float ex = _oceanBounds.max.x - _oceanBounds.min.x;
    float ey = _oceanBounds.max.y - _oceanBounds.min.y;
    float ez = _oceanBounds.max.z - _oceanBounds.min.z;
    float sx = (ex > 0.0f) ? (oMaxX - oMinX) / ex : 1.0f;
    float sz = (ez > 0.0f) ? (oMaxZ - oMinZ) / ez : 1.0f;
    // Hauteur des vagues capée (~0.3 unité) : la surface s'étire, pas le relief.
    float sy = (ey > 0.0f) ? fminf(0.3f / ey, fminf(sx, sz)) : 1.0f;

    Vector3 pos = {
        (oMinX + oMaxX) * 0.5f
            - (_oceanBounds.min.x + _oceanBounds.max.x) * 0.5f * sx,
        OCEAN_TOP_Y - _oceanBounds.max.y * sy,
        (oMinZ + oMaxZ) * 0.5f
            - (_oceanBounds.min.z + _oceanBounds.max.z) * 0.5f * sz,
    };
    DrawModelEx(_ocean, pos, {0.0f, 1.0f, 0.0f}, 0.0f, {sx, sy, sz}, WHITE);
}
