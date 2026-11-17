/*
** EPITECH PROJECT, 2026
** Effects.hpp
** File description:
** Visual effects: particles, shockwaves, flying items, floating texts and
** per-level auras, driven by the FxEvents produced by the protocol parser
*/

#pragma once

#include <raylib.h>
#include <string>
#include <vector>
#include "gui/World.hpp"

class ModelPlaceholder;

class Effects {
public:
    static constexpr size_t MAX_PARTICLES = 3000;
    static constexpr size_t MAX_TEXTS     = 48;

    // Couleur associée à un niveau (1..8) : aura au sol, level-up, incantation.
    static Color levelColor(int level);

    // Consomme world.fxEvents, émet les particules d'ambiance (incantations
    // en cours, auras des hauts niveaux) et fait vieillir tous les effets.
    void update(World &world, const ModelPlaceholder &ph, float tileSize, float dt);

    // À appeler dans BeginMode3D, APRÈS le reste de la scène (effets translucides).
    void draw(const World &world, const ModelPlaceholder &ph, float tileSize) const;

    // À appeler après EndMode3D : textes flottants projetés à l'écran.
    void drawOverlay(const Camera3D &camera) const;

private:
    struct Particle {
        Vector3 pos;
        Vector3 vel;
        Color   color;
        float   life;
        float   maxLife;
        float   size;
        float   gravity;
        float   drag;
        float   grow;     // variation de taille par seconde (fumée qui gonfle)
        bool    additive; // lueur (étincelles) vs. matière (fumée, débris)
    };

    struct Ring {
        Vector3 center;
        float   r0, r1;
        float   thickness;
        Color   color;
        float   life, maxLife;
        float   delay;
    };

    struct Flash {
        Vector3 center;
        float   r0, r1;
        Color   color;
        float   life, maxLife;
    };

    struct Ghost {
        Vector3 pos;
        float   life, maxLife;
        float   phase;
    };

    struct FlyingItem {
        Resource res;
        Vector3  ground;
        Vector3  target;   // pieds du joueur, rafraîchi tant qu'il existe
        int      playerId;
        bool     pickup;   // true : sol -> main, false : main -> sol
        float    t, duration;
    };

    struct FloatText {
        Vector3     pos;
        std::string text;
        Color       color;
        float       life, maxLife;
        int         size;
    };

    std::vector<Particle>   _particles;
    std::vector<Ring>       _rings;
    std::vector<Flash>      _flashes;
    std::vector<Ghost>      _ghosts;
    std::vector<FlyingItem> _items;
    std::vector<FloatText>  _texts;

    float _time       = 0.0f;
    float _charHeight = 1.8f;
    Color _resColors[RESOURCE_COUNT] = {};

    void handleEvent(const World &world, const ModelPlaceholder &ph,
                     float tileSize, const FxEvent &ev);
    void emitAmbient(const World &world, const ModelPlaceholder &ph,
                     float tileSize, float dt);
    bool playerFeet(const World &world, const ModelPlaceholder &ph, int id,
                    float tileSize, Vector3 &out) const;
    Vector3 itemPosition(const FlyingItem &it, float &scale) const;

    void addParticle(const Particle &p);
    void burst(Vector3 c, Color col, int n, float speedMin, float speedMax,
               float upBias, float gravity, float lifeMin, float lifeMax,
               float size, bool additive = true);
    void smoke(Vector3 c, Color col, int n, float spread = 0.3f);
    void spiral(Vector3 c, Color col, int n, float radius);
    void ring(Vector3 c, Color col, float r0, float r1, float thickness,
              float life, float delay = 0.0f);
    void text(Vector3 pos, const std::string &str, Color col, int size,
              float life = 1.8f);
};
