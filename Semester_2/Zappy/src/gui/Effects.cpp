/*
** EPITECH PROJECT, 2026
** Effects.cpp
** File description:
** Visual effects: particles, shockwaves, flying items, floating texts and
** per-level auras, driven by the FxEvents produced by the protocol parser
*/

#include "gui/Effects.hpp"
#include "gui/Lang.hpp"
#include "gui/ModelPlaceholder.hpp"
#include "gui/TeamColors.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <utility>
#include <raymath.h>
#include <rlgl.h>

/* ------------------------------------------------------------------------ */
/* Helpers                                                                  */
/* ------------------------------------------------------------------------ */

static float frand(float a, float b)
{
    return a + (b - a) * (static_cast<float>(GetRandomValue(0, 10000)) / 10000.0f);
}

static Color withAlpha(Color c, float a)
{
    a = Clamp(a, 0.0f, 1.0f);
    c.a = static_cast<unsigned char>(static_cast<float>(c.a) * a);
    return c;
}

static float easeOutCubic(float u)
{
    float v = 1.0f - u;
    return 1.0f - v * v * v;
}

static float easeInOut(float u)
{
    return u * u * (3.0f - 2.0f * u);
}

// Au-dessus de cette hauteur, un anneau peut être vu d'en dessous.
static constexpr float ARC_GROUND_MAX_Y = 0.2f;

static void vertex(Vector3 v)
{
    rlVertex3f(v.x, v.y, v.z);
}

// Arc d'anneau horizontal (plan XZ), en un seul lot de triangles.
// Au ras du sol, la caméra (toujours au-dessus : inclinaison >= 5°) ne voit
// que la face du dessus ; celle du dessous était éliminée par le GPU, elle
// n'est plus envoyée. En hauteur (ondes radio au-dessus des têtes),
// l'anneau reste double face. L'image est la même.
static void drawArc(Vector3 c, float r0, float r1, float a0, float a1,
                    Color col, int seg)
{
    const bool bothSides = c.y > ARC_GROUND_MAX_Y;
    rlBegin(RL_TRIANGLES);
    rlColor4ub(col.r, col.g, col.b, col.a);
    // Chaque angle sert à deux segments voisins : un cos/sin par angle.
    float cos0 = cosf(a0), sin0 = sinf(a0);
    for (int i = 0; i < seg; ++i) {
        float t1 = a0 + (a1 - a0) * static_cast<float>(i + 1) / static_cast<float>(seg);
        float cos1 = cosf(t1), sin1 = sinf(t1);
        Vector3 i0 = {c.x + cos0 * r0, c.y, c.z + sin0 * r0};
        Vector3 o0 = {c.x + cos0 * r1, c.y, c.z + sin0 * r1};
        Vector3 i1 = {c.x + cos1 * r0, c.y, c.z + sin1 * r0};
        Vector3 o1 = {c.x + cos1 * r1, c.y, c.z + sin1 * r1};
        if (bothSides) {
            vertex(i0); vertex(o0); vertex(o1);
        }
        vertex(i0); vertex(o1); vertex(o0);     // dessus
        if (bothSides) {
            vertex(i0); vertex(o1); vertex(i1);
        }
        vertex(i0); vertex(i1); vertex(o1);     // dessus
        cos0 = cos1;
        sin0 = sin1;
    }
    rlEnd();
}

// Les 36 sommets d'un cube unité centré (faces avant vers l'extérieur,
// comme DrawCube), pour émettre les petits cubes sans matrice par cube.
static const std::array<Vector3, 36> &unitCube()
{
    static const std::array<Vector3, 36> verts = [] {
        std::array<Vector3, 36> v{};
        size_t n = 0;
        auto quad = [&](Vector3 a, Vector3 b, Vector3 c, Vector3 d, Vector3 out) {
            Vector3 cross = Vector3CrossProduct(Vector3Subtract(b, a),
                                                Vector3Subtract(c, a));
            if (Vector3DotProduct(cross, out) < 0.0f)
                std::swap(b, d);
            for (Vector3 p : {a, b, c, a, c, d})
                v[n++] = p;
        };
        const float h = 0.5f;
        quad({-h, -h, h}, {h, -h, h}, {h, h, h}, {-h, h, h}, {0, 0, 1});
        quad({-h, -h, -h}, {-h, h, -h}, {h, h, -h}, {h, -h, -h}, {0, 0, -1});
        quad({-h, h, -h}, {-h, h, h}, {h, h, h}, {h, h, -h}, {0, 1, 0});
        quad({-h, -h, -h}, {h, -h, -h}, {h, -h, h}, {-h, -h, h}, {0, -1, 0});
        quad({h, -h, -h}, {h, h, -h}, {h, h, h}, {h, -h, h}, {1, 0, 0});
        quad({-h, -h, -h}, {-h, -h, h}, {-h, h, h}, {-h, h, -h}, {-1, 0, 0});
        return v;
    }();
    return verts;
}

// Équivaut à DrawCube(c, s, s, s, col), mêmes sommets.
static void drawSmallCube(Vector3 c, float s, Color col)
{
    rlBegin(RL_TRIANGLES);
    rlColor4ub(col.r, col.g, col.b, col.a);
    for (const Vector3 &p : unitCube())
        rlVertex3f(c.x + p.x * s, c.y + p.y * s, c.z + p.z * s);
    rlEnd();
}

static void drawRing(Vector3 c, float r0, float r1, Color col, int seg = 40)
{
    drawArc(c, r0, r1, 0.0f, 2.0f * PI, col, seg);
}

static const char *RESOURCE_NAMES_FR[RESOURCE_COUNT] = {
    "Nourriture", "Linemate", "Deraumere", "Sibur",
    "Mendiane", "Phiras", "Thystame"
};
static const char *RESOURCE_NAMES_EN[RESOURCE_COUNT] = {
    "Food", "Linemate", "Deraumere", "Sibur",
    "Mendiane", "Phiras", "Thystame"
};
#define RESOURCE_NAMES (Lang::english() ? RESOURCE_NAMES_EN : RESOURCE_NAMES_FR)

Color Effects::levelColor(int level)
{
    static const Color COLORS[8] = {
        {210, 210, 210, 255}, // 1 : gris clair
        { 80, 225,  90, 255}, // 2 : vert
        { 40, 205, 235, 255}, // 3 : cyan
        { 60, 115, 255, 255}, // 4 : bleu
        {165,  75, 245, 255}, // 5 : violet
        {245,  65, 185, 255}, // 6 : magenta
        {255, 135,  30, 255}, // 7 : orange
        {255, 210,  40, 255}, // 8 : or
    };
    int idx = std::clamp(level, 1, 8) - 1;
    return COLORS[idx];
}

/* ------------------------------------------------------------------------ */
/* Emitters                                                                 */
/* ------------------------------------------------------------------------ */

void Effects::addParticle(const Particle &p)
{
    if (_particles.size() < MAX_PARTICLES)
        _particles.push_back(p);
}

void Effects::burst(Vector3 c, Color col, int n, float speedMin, float speedMax,
                    float upBias, float gravity, float lifeMin, float lifeMax,
                    float size, bool additive)
{
    for (int i = 0; i < n; ++i) {
        float theta = frand(0.0f, 2.0f * PI);
        float y     = frand(-1.0f, 1.0f);
        float r     = sqrtf(1.0f - y * y);
        float speed = frand(speedMin, speedMax);
        Vector3 dir = {r * cosf(theta), y, r * sinf(theta)};
        Particle p{};
        p.pos      = c;
        p.vel      = {dir.x * speed, dir.y * speed + upBias, dir.z * speed};
        p.color    = col;
        p.maxLife  = frand(lifeMin, lifeMax);
        p.life     = p.maxLife;
        p.size     = size * frand(0.6f, 1.2f);
        p.gravity  = gravity;
        p.drag     = 1.2f;
        p.grow     = 0.0f;
        p.additive = additive;
        addParticle(p);
    }
}

void Effects::smoke(Vector3 c, Color col, int n, float spread)
{
    for (int i = 0; i < n; ++i) {
        Particle p{};
        p.pos      = {c.x + frand(-spread, spread), c.y + frand(0.0f, 0.3f),
                      c.z + frand(-spread, spread)};
        p.vel      = {frand(-0.3f, 0.3f), frand(0.4f, 0.9f), frand(-0.3f, 0.3f)};
        p.color    = col;
        p.maxLife  = frand(1.0f, 1.8f);
        p.life     = p.maxLife;
        p.size     = frand(0.10f, 0.20f);
        p.gravity  = 0.0f;
        p.drag     = 1.0f;
        p.grow     = 0.25f;
        p.additive = false;
        addParticle(p);
    }
}

// Particules qui montent en tourbillon autour d'un axe vertical.
void Effects::spiral(Vector3 c, Color col, int n, float radius)
{
    for (int i = 0; i < n; ++i) {
        float a = frand(0.0f, 2.0f * PI);
        Particle p{};
        p.pos      = {c.x + cosf(a) * radius, c.y + frand(0.0f, 0.4f),
                      c.z + sinf(a) * radius};
        // vitesse tangentielle + montée
        p.vel      = {-sinf(a) * 1.6f, frand(1.5f, 3.0f), cosf(a) * 1.6f};
        p.color    = col;
        p.maxLife  = frand(0.8f, 1.4f);
        p.life     = p.maxLife;
        p.size     = frand(0.05f, 0.09f);
        p.gravity  = 0.0f;
        p.drag     = 0.6f;
        p.grow     = 0.0f;
        p.additive = true;
        addParticle(p);
    }
}

void Effects::ring(Vector3 c, Color col, float r0, float r1, float thickness,
                   float life, float delay)
{
    _rings.push_back({c, r0, r1, thickness, col, life, life, delay});
}

void Effects::text(Vector3 pos, const std::string &str, Color col, int size, float life)
{
    // Plusieurs incantateurs d'une même case montent de niveau ensemble :
    // un seul "NIVEAU N" suffit au lieu d'une pile de textes identiques.
    for (const FloatText &t : _texts) {
        if (t.text == str && t.maxLife - t.life < 0.3f
            && Vector3Distance(t.pos, pos) < 1.5f)
            return;
    }
    if (_texts.size() >= MAX_TEXTS)
        _texts.erase(_texts.begin());
    _texts.push_back({pos, str, col, life, life, size});
}

/* ------------------------------------------------------------------------ */
/* Events                                                                   */
/* ------------------------------------------------------------------------ */

bool Effects::playerFeet(const World &world, const ModelPlaceholder &ph, int id,
                         float tileSize, Vector3 &out) const
{
    auto it = world.players.find(id);
    if (it == world.players.end())
        return false;
    Vector2 v = ph.getVisualTilePos(id, it->second.x, it->second.y);
    out = {v.x * tileSize, 0.0f, v.y * tileSize};
    return true;
}

void Effects::handleEvent(const World &world, const ModelPlaceholder &ph,
                          float tileSize, const FxEvent &ev)
{
    Vector3 tile = {static_cast<float>(ev.x) * tileSize, 0.0f,
                    static_cast<float>(ev.y) * tileSize};
    Vector3 feet = tile;
    bool hasPlayer = ev.playerId >= 0
                     && playerFeet(world, ph, ev.playerId, tileSize, feet);
    Vector3 head = {feet.x, feet.y + _charHeight + 0.2f, feet.z};
    bool validRes = ev.value >= 0 && ev.value < RESOURCE_COUNT;

    switch (ev.type) {
    case FxType::Pickup:
    case FxType::Drop: {
        if (!hasPlayer || !validRes)
            break;
        bool pickup = (ev.type == FxType::Pickup);
        float a = frand(0.0f, 2.0f * PI);
        Vector3 ground = {feet.x + cosf(a) * 0.45f, 0.1f, feet.z + sinf(a) * 0.45f};
        Resource res = static_cast<Resource>(ev.value);
        _items.push_back({res, ground, feet, ev.playerId, pickup, 0.0f, 0.55f});
        Color rc = _resColors[res];
        if (pickup) {
            burst(ground, rc, 10, 0.5f, 1.4f, 1.0f, 3.0f, 0.3f, 0.6f, 0.06f);
            text(head, std::string("+") + RESOURCE_NAMES[res], rc, 18, 1.3f);
        } else {
            text(head, std::string("-") + RESOURCE_NAMES[res],
                 ColorLerp(rc, LIGHTGRAY, 0.5f), 16, 1.2f);
        }
        break;
    }
    case FxType::Death: {
        _ghosts.push_back({{feet.x, 0.6f, feet.z}, 2.8f, 2.8f, frand(0.0f, 6.0f)});
        smoke(feet, {60, 60, 60, 200}, 14);
        burst({feet.x, 0.8f, feet.z}, {170, 20, 30, 255}, 18, 0.8f, 2.0f,
              1.0f, 6.0f, 0.4f, 0.9f, 0.07f, false);
        ring({feet.x, 0.04f, feet.z}, {200, 30, 40, 255}, 0.2f, 1.4f, 0.10f, 0.8f);
        text(head, "R.I.P.", {235, 70, 70, 255}, 24, 2.2f);
        break;
    }
    case FxType::Spawn: {
        Color tc = {200, 200, 200, 255};
        auto it = world.players.find(ev.playerId);
        if (it != world.players.end())
            tc = teamColorFromIndex(world.teamIndex(it->second.team));
        burst(feet, tc, 22, 0.3f, 1.0f, 3.0f, 2.0f, 0.6f, 1.1f, 0.07f);
        ring({feet.x, 0.04f, feet.z}, tc, 0.1f, 1.0f, 0.08f, 0.6f);
        break;
    }
    case FxType::LevelUp: {
        Color lc = levelColor(ev.value);
        Vector3 chest = {feet.x, _charHeight * 0.5f, feet.z};
        // Le flash masque l'échange de modèle (skin du nouveau niveau).
        _flashes.push_back({chest, 0.2f, 1.4f, WHITE, 0.55f, 0.55f});
        _flashes.push_back({chest, 0.4f, 1.9f, lc, 0.8f, 0.8f});
        ring({feet.x, 0.05f, feet.z}, lc, 0.2f, 2.6f, 0.14f, 0.9f);
        ring({feet.x, 0.05f, feet.z}, WHITE, 0.2f, 1.8f, 0.06f, 0.7f, 0.15f);
        spiral(feet, lc, 70, 0.6f);
        burst(chest, {255, 230, 120, 255}, 30, 1.5f, 3.5f, 2.0f, 4.0f,
              0.6f, 1.2f, 0.06f);
        text({head.x, head.y + 0.3f, head.z},
             std::string(Lang::T("NIVEAU ", "LEVEL ")) + std::to_string(ev.value), lc, 30, 2.4f);
        break;
    }
    case FxType::IncantStart: {
        Color lc = levelColor(ev.value + 1);
        ring({tile.x, 0.05f, tile.z}, lc, 0.3f, 1.9f, 0.12f, 0.9f);
        ring({tile.x, 0.05f, tile.z}, lc, 0.3f, 1.9f, 0.08f, 0.9f, 0.3f);
        burst({tile.x, 0.2f, tile.z}, lc, 40, 0.5f, 1.5f, 3.0f, 1.0f,
              0.8f, 1.4f, 0.07f);
        text({tile.x, _charHeight + 0.9f, tile.z},
             std::string(Lang::T("Incantation ", "Ritual ")) + std::to_string(ev.value) + " -> "
             + std::to_string(ev.value + 1), lc, 20, 2.0f);
        break;
    }
    case FxType::IncantEnd: {
        // Plus haut que "NIVEAU N" (head + 0.5) qui arrive au même moment.
        Vector3 top = {tile.x, _charHeight + 1.6f, tile.z};
        if (ev.value != 0) {
            Color gold = {255, 210, 40, 255};
            _flashes.push_back({{tile.x, 0.8f, tile.z}, 0.3f, 2.0f, gold, 0.7f, 0.7f});
            burst({tile.x, 0.5f, tile.z}, gold, 70, 2.0f, 5.0f, 3.0f, 5.0f,
                  0.8f, 1.6f, 0.08f);
            burst({tile.x, 0.5f, tile.z}, WHITE, 30, 1.0f, 3.0f, 4.0f, 3.0f,
                  0.6f, 1.2f, 0.05f);
            ring({tile.x, 0.06f, tile.z}, gold, 0.2f, 3.2f, 0.18f, 1.0f);
            ring({tile.x, 0.06f, tile.z}, WHITE, 0.2f, 2.4f, 0.08f, 0.8f, 0.2f);
            text(top, Lang::T("Elevation reussie !", "Level up!"), gold, 26, 2.4f);
        } else {
            smoke(tile, {90, 90, 90, 210}, 26, 0.6f);
            burst({tile.x, 0.6f, tile.z}, {150, 30, 30, 255}, 20, 0.5f, 1.5f,
                  0.5f, 6.0f, 0.5f, 0.9f, 0.06f, false);
            ring({tile.x, 0.06f, tile.z}, {220, 40, 40, 255}, 0.2f, 1.6f, 0.10f, 0.8f);
            text(top, Lang::T("Incantation ratee", "Ritual failed"),
                 {235, 70, 70, 255}, 22, 2.2f);
        }
        break;
    }
    case FxType::Broadcast: {
        if (!hasPlayer)
            break;
        Color bc = {120, 220, 255, 255};
        for (int i = 0; i < 3; ++i)
            ring(head, bc, 0.2f, 2.6f, 0.06f, 1.1f, 0.25f * static_cast<float>(i));
        break;
    }
    case FxType::Eject: {
        ring({feet.x, 0.06f, feet.z}, {255, 150, 40, 255}, 0.3f, 2.4f, 0.16f, 0.5f);
        for (int i = 0; i < 20; ++i) {
            float a = frand(0.0f, 2.0f * PI);
            float s = frand(2.0f, 4.0f);
            Particle p{};
            p.pos      = {feet.x, 0.1f, feet.z};
            p.vel      = {cosf(a) * s, frand(0.2f, 0.8f), sinf(a) * s};
            p.color    = {150, 115, 75, 200};
            p.maxLife  = frand(0.5f, 0.9f);
            p.life     = p.maxLife;
            p.size     = frand(0.08f, 0.14f);
            p.gravity  = 1.0f;
            p.drag     = 3.0f;
            p.grow     = 0.3f;
            p.additive = false;
            addParticle(p);
        }
        text(head, Lang::T("Expulsion !", "Ejected!"), {255, 150, 40, 255}, 20, 1.5f);
        break;
    }
    case FxType::Fork: {
        Color pink = {255, 140, 200, 255};
        burst({feet.x, 0.5f, feet.z}, pink, 24, 0.5f, 1.5f, 1.5f, 1.5f,
              0.6f, 1.1f, 0.06f);
        text(head, Lang::T("Ponte", "Egg laid"), pink, 18, 1.4f);
        break;
    }
    case FxType::EggLaid:
        ring({tile.x, 0.05f, tile.z}, WHITE, 0.1f, 0.8f, 0.06f, 0.5f);
        burst({tile.x, 0.2f, tile.z}, {255, 240, 200, 255}, 12, 0.4f, 1.0f,
              1.0f, 2.0f, 0.4f, 0.7f, 0.05f);
        break;
    case FxType::EggHatch:
        _flashes.push_back({{tile.x, 0.2f, tile.z}, 0.1f, 0.8f, WHITE, 0.4f, 0.4f});
        // éclats de coquille (matière, retombent)
        burst({tile.x, 0.2f, tile.z}, {240, 232, 205, 255}, 18, 1.5f, 3.0f,
              2.0f, 9.0f, 0.6f, 1.0f, 0.07f, false);
        burst({tile.x, 0.3f, tile.z}, {255, 230, 90, 255}, 20, 0.5f, 1.5f,
              2.0f, 1.0f, 0.6f, 1.0f, 0.05f);
        text({tile.x, 1.2f, tile.z}, Lang::T("Eclosion", "Hatching"),
             {255, 230, 90, 255}, 18, 1.5f);
        break;
    case FxType::EggDie:
        smoke(tile, {120, 120, 120, 200}, 10, 0.2f);
        break;
    }
}

// Effets continus : incantations en cours et étincelles des hauts niveaux.
void Effects::emitAmbient(const World &world, const ModelPlaceholder &ph,
                          float tileSize, float dt)
{
    for (const auto &row : world.tiles) {
        for (const Tile &t : row) {
            if (!t.isIncanting)
                continue;
            Color lc = levelColor(t.incantationLevel + 1);
            float expected = 35.0f * dt + frand(0.0f, 1.0f);
            int n = static_cast<int>(expected);
            Vector3 c = {static_cast<float>(t.x) * tileSize, 0.1f,
                         static_cast<float>(t.y) * tileSize};
            for (int i = 0; i < n; ++i) {
                float a = frand(0.0f, 2.0f * PI);
                float r = frand(0.2f, 0.9f);
                Particle p{};
                p.pos      = {c.x + cosf(a) * r, c.y, c.z + sinf(a) * r};
                p.vel      = {0.0f, frand(1.2f, 2.6f), 0.0f};
                p.color    = lc;
                p.maxLife  = frand(1.0f, 1.8f);
                p.life     = p.maxLife;
                p.size     = frand(0.04f, 0.08f);
                p.gravity  = 0.0f;
                p.drag     = 0.2f;
                p.grow     = 0.0f;
                p.additive = true;
                addParticle(p);
            }
        }
    }

    for (const auto &pair : world.players) {
        const Player &p = pair.second;
        if (!p.alive || p.level < 6)
            continue;
        float rate = (p.level >= 8) ? 10.0f : 3.0f;
        int n = static_cast<int>(rate * dt + frand(0.0f, 1.0f));
        if (n <= 0)
            continue;
        Vector3 feet;
        if (!playerFeet(world, ph, p.id, tileSize, feet))
            continue;
        Color lc = levelColor(p.level);
        for (int i = 0; i < n; ++i) {
            float a = frand(0.0f, 2.0f * PI);
            Particle q{};
            q.pos      = {feet.x + cosf(a) * 0.45f, frand(0.1f, _charHeight),
                          feet.z + sinf(a) * 0.45f};
            q.vel      = {0.0f, frand(0.3f, 0.8f), 0.0f};
            q.color    = lc;
            q.maxLife  = frand(0.7f, 1.2f);
            q.life     = q.maxLife;
            q.size     = 0.045f;
            q.gravity  = 0.0f;
            q.drag     = 0.0f;
            q.grow     = 0.0f;
            q.additive = true;
            addParticle(q);
        }
    }
}

/* ------------------------------------------------------------------------ */
/* Update                                                                   */
/* ------------------------------------------------------------------------ */

void Effects::update(World &world, const ModelPlaceholder &ph, float tileSize, float dt)
{
    _time      += dt;
    _charHeight = ph.getCharacterHeight();
    const Color *rc = ph.getResourceColors();
    for (int i = 0; i < RESOURCE_COUNT; ++i)
        _resColors[i] = rc[i];

    for (const FxEvent &ev : world.fxEvents)
        handleEvent(world, ph, tileSize, ev);
    world.fxEvents.clear();

    emitAmbient(world, ph, tileSize, dt);

    for (Particle &p : _particles) {
        p.vel.y -= p.gravity * dt;
        float k = std::max(0.0f, 1.0f - p.drag * dt);
        p.vel    = Vector3Scale(p.vel, k);
        p.pos    = Vector3Add(p.pos, Vector3Scale(p.vel, dt));
        if (p.pos.y < 0.02f && p.gravity > 0.0f) {
            p.pos.y = 0.02f;
            p.vel   = {p.vel.x * 0.5f, -p.vel.y * 0.3f, p.vel.z * 0.5f};
        }
        p.size += p.grow * dt;
        p.life -= dt;
    }
    _particles.erase(std::remove_if(_particles.begin(), _particles.end(),
        [](const Particle &p) { return p.life <= 0.0f; }), _particles.end());

    for (Ring &r : _rings) {
        if (r.delay > 0.0f)
            r.delay -= dt;
        else
            r.life -= dt;
    }
    _rings.erase(std::remove_if(_rings.begin(), _rings.end(),
        [](const Ring &r) { return r.life <= 0.0f; }), _rings.end());

    for (Flash &f : _flashes)
        f.life -= dt;
    _flashes.erase(std::remove_if(_flashes.begin(), _flashes.end(),
        [](const Flash &f) { return f.life <= 0.0f; }), _flashes.end());

    for (Ghost &g : _ghosts) {
        g.life  -= dt;
        g.pos.y += 0.9f * dt;
    }
    _ghosts.erase(std::remove_if(_ghosts.begin(), _ghosts.end(),
        [](const Ghost &g) { return g.life <= 0.0f; }), _ghosts.end());

    std::vector<FlyingItem> landed;
    for (FlyingItem &it : _items) {
        Vector3 feet;
        if (playerFeet(world, ph, it.playerId, tileSize, feet))
            it.target = feet;
        it.t += dt;
        if (it.t >= it.duration)
            landed.push_back(it);
    }
    for (const FlyingItem &it : landed) {
        if (it.pickup) {
            Vector3 hand = {it.target.x, _charHeight * 0.55f, it.target.z};
            burst(hand, _resColors[it.res], 10, 0.4f, 1.2f, 0.5f, 0.0f,
                  0.3f, 0.5f, 0.05f);
        } else {
            smoke(it.ground, {150, 120, 85, 170}, 5, 0.1f);
        }
    }
    _items.erase(std::remove_if(_items.begin(), _items.end(),
        [](const FlyingItem &it) { return it.t >= it.duration; }), _items.end());

    for (FloatText &t : _texts)
        t.life -= dt;
    _texts.erase(std::remove_if(_texts.begin(), _texts.end(),
        [](const FloatText &t) { return t.life <= 0.0f; }), _texts.end());
}

/* ------------------------------------------------------------------------ */
/* Draw                                                                     */
/* ------------------------------------------------------------------------ */

Vector3 Effects::itemPosition(const FlyingItem &it, float &scale) const
{
    float u = Clamp(it.t / it.duration, 0.0f, 1.0f);
    Vector3 hand = {it.target.x, _charHeight * 0.55f, it.target.z};
    Vector3 from = it.pickup ? it.ground : hand;
    Vector3 to   = it.pickup ? hand : it.ground;
    Vector3 p    = Vector3Lerp(from, to, easeInOut(u));
    p.y += sinf(u * PI) * 0.9f;
    scale = it.pickup ? (1.0f - 0.7f * u) : (0.3f + 0.7f * u);
    return p;
}

void Effects::draw(const World &world, const ModelPlaceholder &ph, float tileSize) const
{
    // 1. Objets ramassés/posés : opaques, avec écriture de profondeur.
    for (const FlyingItem &it : _items) {
        float scale = 1.0f;
        Vector3 p = itemPosition(it, scale);
        ph.drawResource(it.res, p, scale);
    }

    // 2. Tout le reste est translucide : on garde le test de profondeur
    //    (caché derrière les modèles) mais sans l'écrire, pour éviter que
    //    les effets se masquent entre eux.
    rlDrawRenderBatchActive();
    rlDisableDepthMask();

    // --- passe "matière" (mélange alpha) : fumée, débris, fantômes ---
    for (const Particle &p : _particles) {
        if (p.additive)
            continue;
        float a = p.life / p.maxLife;
        DrawSphereEx(p.pos, p.size, 4, 6, withAlpha(p.color, a));
    }
    for (const Ghost &g : _ghosts) {
        float u = 1.0f - g.life / g.maxLife;
        float a = (u < 0.15f) ? u / 0.15f : (1.0f - u);
        Color body = withAlpha({235, 240, 255, 170}, a);
        Vector3 head = {g.pos.x + sinf(_time * 3.0f + g.phase) * 0.15f,
                        g.pos.y + 0.45f, g.pos.z};
        Vector3 tail = {g.pos.x, g.pos.y - 0.2f, g.pos.z};
        DrawCylinderEx(tail, head, 0.03f, 0.24f, 10, body);
        DrawSphereEx(head, 0.24f, 8, 10, body);
    }

    // --- passe "lumière" (additive) ---
    BeginBlendMode(BLEND_ADDITIVE);

    // Auras de niveau : anneau au sol + lucioles en orbite (level - 1).
    for (const auto &pair : world.players) {
        const Player &pl = pair.second;
        if (!pl.alive)
            continue;
        Vector3 feet;
        if (!playerFeet(world, ph, pl.id, tileSize, feet))
            continue;
        Color lc    = levelColor(pl.level);
        float phase = static_cast<float>(pl.id) * 1.7f;
        float pulse = 0.5f + 0.5f * sinf(_time * 3.0f + phase);
        Vector3 g   = {feet.x, 0.04f, feet.z};
        float thick = 0.05f + 0.012f * static_cast<float>(pl.level);
        drawRing(g, 0.52f, 0.52f + thick, withAlpha(lc, 0.45f + 0.35f * pulse), 32);
        if (pl.level >= 4)
            drawRing(g, 0.0f, 0.5f, withAlpha(lc, 0.10f + 0.08f * pulse), 24);
        if (pl.level >= 6) {
            // runes tournantes autour de l'anneau
            float rot = _time * 1.2f + phase;
            for (int k = 0; k < 4; ++k) {
                float a0 = rot + static_cast<float>(k) * PI * 0.5f;
                drawArc(g, 0.68f, 0.74f, a0, a0 + PI * 0.3f, withAlpha(lc, 0.7f), 6);
            }
        }
        int motes = pl.level - 1;
        for (int m = 0; m < motes; ++m) {
            float a = _time * 1.8f + phase
                    + 2.0f * PI * static_cast<float>(m) / static_cast<float>(motes);
            float h = _charHeight * (0.35f + 0.25f * sinf(_time * 2.0f + a));
            Vector3 mp = {feet.x + cosf(a) * 0.65f, h, feet.z + sinf(a) * 0.65f};
            float s = 0.06f + 0.006f * static_cast<float>(pl.level);
            drawSmallCube(mp, s, withAlpha(lc, 0.9f));
        }
        if (pl.level >= 8) {
            // colonne dorée du niveau maximal
            DrawCylinder(g, 0.55f, 0.35f, _charHeight * 1.6f, 16,
                         withAlpha(lc, 0.10f + 0.06f * pulse));
        }
    }

    // Incantations en cours : pilier de lumière + cercle de runes.
    for (const auto &row : world.tiles) {
        for (const Tile &t : row) {
            if (!t.isIncanting)
                continue;
            Color lc = levelColor(t.incantationLevel + 1);
            Vector3 c = {static_cast<float>(t.x) * tileSize, 0.05f,
                         static_cast<float>(t.y) * tileSize};
            float pulse = 0.5f + 0.5f * sinf(_time * 5.0f);
            DrawCylinder(c, 0.85f, 0.85f, 7.0f, 20, withAlpha(lc, 0.10f + 0.10f * pulse));
            DrawCylinder(c, 0.35f, 0.35f, 7.0f, 12, withAlpha(WHITE, 0.10f + 0.08f * pulse));
            float rot = _time * 2.0f;
            for (int k = 0; k < 6; ++k) {
                float a0 = rot + static_cast<float>(k) * PI / 3.0f;
                drawArc(c, 0.9f, 1.0f, a0, a0 + PI / 4.5f, withAlpha(lc, 0.9f), 6);
            }
            drawRing(c, 0.0f, 0.95f, withAlpha(lc, 0.12f + 0.10f * pulse), 32);
        }
    }

    for (const Ring &r : _rings) {
        if (r.delay > 0.0f)
            continue;
        float u   = 1.0f - r.life / r.maxLife;
        float rad = Lerp(r.r0, r.r1, easeOutCubic(u));
        drawRing(r.center, rad, rad + r.thickness, withAlpha(r.color, 1.0f - u), 40);
    }
    for (const Flash &f : _flashes) {
        float u = 1.0f - f.life / f.maxLife;
        float rad = Lerp(f.r0, f.r1, easeOutCubic(u));
        DrawSphereEx(f.center, rad, 10, 12, withAlpha(f.color, 0.55f * (1.0f - u)));
    }
    for (const Particle &p : _particles) {
        if (!p.additive)
            continue;
        float a = p.life / p.maxLife;
        float s = p.size * std::min(1.0f, 3.0f * a);
        drawSmallCube(p.pos, s, withAlpha(p.color, a));
    }

    EndBlendMode();
    rlDrawRenderBatchActive();
    rlEnableDepthMask();
}

void Effects::drawOverlay(const Camera3D &camera) const
{
    Vector3 fwd = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    for (const FloatText &t : _texts) {
        float u = 1.0f - t.life / t.maxLife;
        Vector3 p = t.pos;
        p.y += u * 1.2f;
        Vector3 toP = Vector3Subtract(p, camera.position);
        float depth = Vector3DotProduct(toP, fwd);
        if (depth <= 0.1f)
            continue;
        Vector2 s = GetWorldToScreen(p, camera);

        float alpha = (u < 0.6f) ? 1.0f : (1.0f - u) / 0.4f;
        float pop   = (u < 0.08f) ? 0.5f + u / 0.08f * 0.6f
                    : (u < 0.15f ? 1.1f - (u - 0.08f) / 0.07f * 0.1f : 1.0f);
        float dist  = Clamp(18.0f / depth, 0.6f, 1.4f);
        int   fs    = std::max(10, static_cast<int>(static_cast<float>(t.size) * pop * dist));
        int   w     = MeasureText(t.text.c_str(), fs);
        int   x     = static_cast<int>(s.x) - w / 2;
        int   y     = static_cast<int>(s.y) - fs / 2;

        Color outline = withAlpha({0, 0, 0, 220}, alpha);
        DrawText(t.text.c_str(), x - 2, y, fs, outline);
        DrawText(t.text.c_str(), x + 2, y, fs, outline);
        DrawText(t.text.c_str(), x, y - 2, fs, outline);
        DrawText(t.text.c_str(), x, y + 2, fs, outline);
        DrawText(t.text.c_str(), x, y, fs, withAlpha(t.color, alpha));
    }
}
