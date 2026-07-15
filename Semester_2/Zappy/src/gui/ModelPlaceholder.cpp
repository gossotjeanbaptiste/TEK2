/*
** EPITECH PROJECT, 2026
** ModelPlaceholder.cpp
*/

#include "gui/ModelPlaceholder.hpp"
#include <iostream>
#include <cmath>
#include <raymath.h>

ModelPlaceholder::ModelPlaceholder() : _playerModelLoaded(false)
{
    // 1. Couleurs de fallback
    _resourceColors[FOOD]      = LIME;
    _resourceColors[LINEMATE]  = RED;
    _resourceColors[DERAUMERE] = BLUE;
    _resourceColors[SIBUR]     = ORANGE;
    _resourceColors[MENDIANE]  = PURPLE;
    _resourceColors[PHIRAS]    = PINK;
    _resourceColors[THYSTAME]  = GOLD;


    _resourceScales[FOOD]      = 1.00f; //1.00
    _resourceScales[LINEMATE]  = 0.05f; //0.05
    _resourceScales[DERAUMERE] = 0.30f; //0.30
    _resourceScales[SIBUR]     = 4.30f; //4.30
    _resourceScales[MENDIANE]  = 2.30f; //2.30
    _resourceScales[PHIRAS]    = 4.30f; //4.30
    _resourceScales[THYSTAME]  = 0.30f; //0.30
}

void ModelPlaceholder::loadAll()
{
    std::clog << "[Asset] Loading 3D models onto GPU...\n";

    loadAsset(FOOD,      "assets/models/Whole Ham.glb");
    loadAsset(LINEMATE,  "assets/models/Crystal.glb");
    loadAsset(DERAUMERE, "assets/models/Big Crystal.glb");
    loadAsset(SIBUR,     "assets/models/Crystal Rock.glb");
    loadAsset(MENDIANE,  "assets/models/Rock.glb");
    loadAsset(PHIRAS,    "assets/models/Crystal Rock.glb");
    loadAsset(THYSTAME,  "assets/models/Big Crystal.glb");

    _playerModelLoaded = loadModelFile(_playerModel, "assets/models/Character_Base.glb");
    if (_playerModelLoaded)
        centerPlayerModel();
}

void ModelPlaceholder::centerPlayerModel()
{
    BoundingBox box = GetModelBoundingBox(_playerModel);

    _playerModel.transform = MatrixTranslate(
        -(box.min.x + box.max.x) / 2.0f,
        -box.min.y,
        -(box.min.z + box.max.z) / 2.0f);
}

void ModelPlaceholder::unloadAll()
{
    for (auto &pair : _models) {
        UnloadModel(pair.second);
    }
    _models.clear();
    if (_playerModelLoaded) {
        UnloadModel(_playerModel);
        _playerModelLoaded = false;
    }
}

ModelPlaceholder::~ModelPlaceholder()
{
    // Libérer un modèle exige un contexte OpenGL vivant : après
    // CloseWindow(), on laisse l'OS récupérer la mémoire du process
    if (IsWindowReady())
        unloadAll();
}

bool ModelPlaceholder::loadModelFile(Model &model, const std::string &path)
{
    if (!FileExists(path.c_str())) {
        std::clog << "[Asset Warning] File not found: " << path << " -> Using geometric placeholder.\n";
        return false;
    }
    model = LoadModel(path.c_str());
    return true;
}

void ModelPlaceholder::loadAsset(Resource type, const std::string &path)
{
    Model m;
    if (loadModelFile(m, path)) {
        _models[type] = m;
    }
}

void ModelPlaceholder::drawResource(Resource type, Vector3 position) const
{
    auto it = _models.find(type);
    if (it != _models.end()) {
        Color tint = (type == FOOD) ? WHITE : _resourceColors[type];
        
        // On récupère le scale configuré spécifiquement pour ce type d'objet !
        float currentScale = _resourceScales[type];
        
        DrawModel(it->second, position, currentScale, tint);
    } else {
        // Fallback géométrique (si le .glb est manquant)
        float radius = 0.15f;
        DrawSphere(position, radius, _resourceColors[type]);
        DrawSphereWires(position, radius, 4, 4, DARKGRAY);
    }
}

void ModelPlaceholder::drawPlayer(Vector3 position, int orientation, Color teamColor) const
{
    float rotationAngle = 0.0f;
    if (orientation == 1) rotationAngle = 180.0f;
    if (orientation == 2) rotationAngle = 90.0f;
    if (orientation == 3) rotationAngle = 0.0f;
    if (orientation == 4) rotationAngle = 270.0f;

    if (_playerModelLoaded) {
        Vector3 modelScale = {0.5f, 0.5f, 0.5f};
        DrawModelEx(_playerModel, position, {0.0f, 1.0f, 0.0f}, rotationAngle, modelScale, teamColor);
    } else {
        Vector3 size = {0.4f, 0.6f, 0.4f};
        Vector3 center = position;
        center.y += size.y / 2.0f; // Le cube est posé au sol, pas centré dessus
        DrawCube(center, size.x, size.y, size.z, teamColor);
        DrawCubeWires(center, size.x, size.y, size.z, BLACK);

        Vector3 nosePos = center;
        float rad = (rotationAngle + 90.0f) * DEG2RAD;
        nosePos.x += sinf(rad) * 0.3f;
        nosePos.z += cosf(rad) * 0.3f;
        DrawSphere(nosePos, 0.1f, RAYWHITE);
    }
}

void ModelPlaceholder::drawEgg(Vector3 position) const
{
    DrawSphere(position, 0.2f, RAYWHITE);
    DrawSphereWires(position, 0.2f, 5, 5, LIGHTGRAY);
}