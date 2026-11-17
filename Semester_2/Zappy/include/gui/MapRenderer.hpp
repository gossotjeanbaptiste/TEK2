/*
** EPITECH PROJECT, 2026
** MapRenderer.hpp
*/

#pragma once
#include "gui/World.hpp"
#include "gui/ModelPlaceholder.hpp"
#include "gui/Decor.hpp"
#include "gui/Effects.hpp"

class MapRenderer {
public:
    MapRenderer() = default;
    ~MapRenderer();
    MapRenderer(const MapRenderer &) = delete;
    MapRenderer &operator=(const MapRenderer &) = delete;

    
    static constexpr float TILE_SIZE = 2.0f; 

    // Les effets passent avant updatePlayers : celui-ci retire les morts
    // de la carte, et l'effet de mort a encore besoin de leur position.
    void update(World &world, float deltaTime)
    {
        _effects.update(world, _placeholder, TILE_SIZE, deltaTime);
        _placeholder.updatePlayers(world, deltaTime);
    }
    // Textes flottants des effets : à appeler après EndMode3D.
    void drawOverlay(const Camera3D &camera) const { _effects.drawOverlay(camera); }
    // (hoverX, hoverY) : case survolée à surligner, -1 si aucune.
    void draw(const World &world, int hoverX = -1, int hoverY = -1);
    void loadAssets()
    {
        _placeholder.loadAll();
        _decor.loadAll(_placeholder.getCharacterHeight());
    }
    const Color* getResourceColors() const { return _resourceColors; }

    // Position affichée du joueur (interpolée pendant la marche), en cases :
    // sert au mode "watch" pour que la caméra suive le glissement du modèle.
    Vector2 getPlayerVisualPos(int id, int fallbackX, int fallbackY) const
    {
        return _placeholder.getVisualTilePos(id, fallbackX, fallbackY);
    }

    void  setPlayerScale(float scale) { _placeholder.setPlayerScale(scale); }
    float getPlayerScale() const { return _placeholder.getPlayerScale(); }

private:
    ModelPlaceholder _placeholder;
    Decor _decor;
    Effects _effects;
    Color _resourceColors[7];

    // Sol de toutes les cases en un mesh statique (reconstruit si la carte
    // change de taille) ; _floorIncanting retient la couleur de chaque case.
    Mesh     _floor{};
    Material _floorMat{};
    bool     _floorBuilt     = false;
    bool     _floorMatLoaded = false;
    int      _floorW         = 0;
    int      _floorH         = 0;
    std::vector<unsigned char> _floorIncanting;

    // Positions de la frame, réutilisées pour éviter les allocations.
    std::vector<Vector3> _resourcePositions[RESOURCE_COUNT];
    std::vector<Vector3> _eggPositions;

    void buildFloor(int width, int height);
    void syncFloorColors(const World &world);
    void drawGridLines(const World &world) const;
};