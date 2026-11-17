/*
** EPITECH PROJECT, 2026
** Decor.hpp
** File description:
** Procedural decorative ring around the game map (Kenney Survival Kit)
*/

#pragma once

#include <raylib.h>
#include <string>
#include <utility>
#include <vector>

// Décor purement cosmétique généré en anneau autour de la carte de jeu :
// sol étendu, végétation/rochers dispersés et un petit campement.
class Decor {
public:
    // Largeur de l'anneau de décor autour de la carte, en cases.
    static constexpr int RING_DEPTH = 5;

    // Probabilité qu'une case de l'anneau reçoive un élément de décor.
    static constexpr float PROP_DENSITY = 0.35f;

    // Hauteur du sol du décor (le dessus des cases de jeu est à y = 0).
    static constexpr float GROUND_Y = -0.05f;

    // L'île est entourée d'une plage de sable, elle-même entourée d'océan.
    // BEACH_DEPTH : largeur de la plage en cases ; l'accès à la plage (chemin
    // de sable) traverse l'anneau de décor sur le bord ouest, le seul libre
    // (campement au nord, village au sud, château à l'est).
    static constexpr int   BEACH_DEPTH  = 4;
    static constexpr float BEACH_TOP_Y  = -0.18f;
    static constexpr float OCEAN_TOP_Y  = -0.45f;
    static constexpr int   OCEAN_EXTENT = 70; // étendue au-delà de la plage

    Decor() = default;
    ~Decor();

    // characterHeight : hauteur du personnage joueur (unités monde), étalon
    // de toutes les tailles du décor (exprimées en multiples de celle-ci).
    void loadAll(float characterHeight);

    // (Re)génère les instances si la taille de la carte a changé.
    void ensureBuilt(int mapWidth, int mapHeight, float tileSize);

    void draw() const;

private:
    struct DecorModel {
        Model model{};
        float baseScale = 1.0f; // normalisation vers la taille cible
        float minY      = 0.0f; // pour poser le bas du modèle sur le sol
        bool  loaded    = false;
    };

    struct Instance {
        int     modelIndex;
        Vector3 position;
        float   yawDeg;
        float   scale;
    };

    std::vector<DecorModel> _models;
    std::vector<std::pair<int, int>> _scatter; // {index modèle, poids de tirage}
    std::vector<Instance> _instances;
    // Matrices des instances, rangées par modèle (index de _models).
    std::vector<std::vector<Matrix>> _batches;

    // Océan : dessiné à part (échelle non uniforme pour couvrir la zone
    // sans étirer la hauteur des vagues).
    Model       _ocean{};
    BoundingBox _oceanBounds{};
    bool        _oceanLoaded = false;

    int   _builtW     = 0;
    int   _builtH     = 0;
    float _tileSize   = 2.0f;
    float _charHeight = 1.8f;

    // Modèles du campement (nord)
    int _tent      = -1;
    int _campfire  = -1;
    int _barrel    = -1;
    int _chest     = -1;
    int _signpost  = -1;
    int _workbench = -1;
    int _bedroll   = -1;

    // Modèles du village médiéval (sud)
    int _house1 = -1;
    int _house2 = -1;
    int _house4 = -1;
    int _inn    = -1;
    int _well   = -1;
    int _mill   = -1;

    // Modèles du château (est)
    int _castle     = -1;
    int _watchTower = -1;

    // targetHeights : taille cible en multiples de la hauteur du personnage.
    int  loadOne(const std::string &relPath, float targetHeights);
    void addInstance(int modelIndex, float tileX, float tileY, float yawDeg, float scaleMul);
    void rebuildBatches();
    void drawBeach(float minX, float maxX, float minZ, float maxZ) const;
    void drawOcean(float minX, float maxX, float minZ, float maxZ) const;
};
