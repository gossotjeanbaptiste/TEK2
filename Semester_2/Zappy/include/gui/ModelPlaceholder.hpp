/*
** EPITECH PROJECT, 2026
** ModelPlaceholder.hpp
*/

#pragma once

#include <raylib.h>
#include <string>
#include <unordered_map>
#include "Tile.hpp"

class ModelPlaceholder {
public:
    ModelPlaceholder();
    ~ModelPlaceholder();

    void loadAll();
    void unloadAll();
    void drawResource(Resource type, Vector3 position) const;
    void drawPlayer(Vector3 position, int orientation, Color teamColor) const;
    void drawEgg(Vector3 position) const;

private:
    Color _resourceColors[RESOURCE_COUNT];
    float _resourceScales[RESOURCE_COUNT];
    std::unordered_map<Resource, Model> _models;
    
    Model _playerModel;
    bool  _playerModelLoaded;

    bool loadModelFile(Model &model, const std::string &path);
    void loadAsset(Resource type, const std::string &path);
    void centerPlayerModel();
};