/*
** EPITECH PROJECT, 2026
** MapRenderer.hpp
*/

#pragma once
#include "gui/World.hpp"
#include "gui/ModelPlaceholder.hpp"

class MapRenderer {
public:
    MapRenderer() = default;
    ~MapRenderer() = default;

    
    static constexpr float TILE_SIZE = 2.0f; 

    void draw(const World &world) const;
    void loadAssets() { _placeholder.loadAll(); }
    void unloadAssets() { _placeholder.unloadAll(); }
    const Color* getResourceColors() const { return _resourceColors; }

private:
    ModelPlaceholder _placeholder;
    Color _resourceColors[7]; 
};