/*
** EPITECH PROJECT, 2026
** ModelPlaceholder.cpp
** File description:
** 3D model loading, character animation state machine and rendering
*/

#include "gui/ModelPlaceholder.hpp"
#include "gui/GpuDraw.hpp"
#include <iostream>
#include <cmath>
#include <raymath.h>
#include <rlgl.h>

static void applyOrientationFix(Model &model)
{
    model.transform = MatrixRotateXYZ({
        ModelPlaceholder::PLAYER_FIX_ROTATE_X * DEG2RAD,
        ModelPlaceholder::PLAYER_FIX_ROTATE_Y * DEG2RAD,
        ModelPlaceholder::PLAYER_FIX_ROTATE_Z * DEG2RAD
    });
}

ModelPlaceholder::ModelPlaceholder() : _playerModelLoaded(false)
{
    _resourceColors[FOOD]      = LIME;
    _resourceColors[LINEMATE]  = RED;
    _resourceColors[DERAUMERE] = BLUE;
    _resourceColors[SIBUR]     = ORANGE;
    _resourceColors[MENDIANE]  = PURPLE;
    _resourceColors[PHIRAS]    = PINK;
    _resourceColors[THYSTAME]  = GOLD;

    _resourceScales[FOOD]      = 1.00f;
    _resourceScales[LINEMATE]  = 0.05f;
    _resourceScales[DERAUMERE] = 0.30f;
    _resourceScales[SIBUR]     = 4.30f;
    _resourceScales[MENDIANE]  = 2.30f;
    _resourceScales[PHIRAS]    = 4.30f;
    _resourceScales[THYSTAME]  = 0.30f;
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
    if (_playerModelLoaded) {
        _playerBounds = GetModelBoundingBox(_playerModel);
        applyOrientationFix(_playerModel);
    }

    _gemLoaded = loadModelFile(_gemModel, "assets/models/Gem Pink.glb");
    if (_gemLoaded) {
        BoundingBox gb = GetModelBoundingBox(_gemModel);
        float largest = fmaxf(gb.max.x - gb.min.x,
                        fmaxf(gb.max.y - gb.min.y, gb.max.z - gb.min.z));
        _gemScale = (largest > 0.0f) ? GEM_SIZE / largest : 1.0f;
        // Matériau neutralisé en blanc : la teinte DrawModelEx donne alors
        // exactement la couleur d'équipe au lieu de se mélanger au rose.
        // Une éventuelle texture diffuse (rose) est aussi remplacée par la
        // texture blanche par défaut, sinon elle se multiplierait à la teinte.
        Texture2D blank = {rlGetTextureIdDefault(), 1, 1, 1,
                           PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};
        for (int i = 0; i < _gemModel.materialCount; ++i) {
            MaterialMap &map = _gemModel.materials[i].maps[MATERIAL_MAP_DIFFUSE];
            map.color = WHITE;
            if (map.texture.id != 0 && map.texture.id != blank.id) {
                unsigned int old = map.texture.id;
                UnloadTexture(map.texture);
                // la même texture peut être partagée par plusieurs matériaux
                for (int j = i; j < _gemModel.materialCount; ++j) {
                    MaterialMap &other = _gemModel.materials[j].maps[MATERIAL_MAP_DIFFUSE];
                    if (other.texture.id == old)
                        other.texture = blank;
                }
            }
            map.texture = blank;
        }
    }

    // Couronne de la reine : contrairement à la gemme, le matériau n'est PAS
    // neutralisé — elle garde ses couleurs d'origine (teinte WHITE au dessin).
    _crownLoaded = loadModelFile(_crownModel, "assets/models/Crown.glb");
    if (_crownLoaded) {
        BoundingBox cb = GetModelBoundingBox(_crownModel);
        float largest = fmaxf(cb.max.x - cb.min.x,
                        fmaxf(cb.max.y - cb.min.y, cb.max.z - cb.min.z));
        _crownScale   = (largest > 0.0f) ? CROWN_SIZE / largest : 1.0f;
        _crownCenterY = (cb.min.y + cb.max.y) * 0.5f;
    }

    // Œuf : une sphère partagée, dessinée par instanciation (DrawSphere
    // recalculait ses ~1700 sommets à chaque œuf et à chaque frame).
    _eggModel  = LoadModelFromMesh(GenMeshSphere(EGG_RADIUS, 16, 16));
    _eggLoaded = true;

    static const char *MAN_PATHS[LEVEL_COUNT] = {
        "assets/models/Character/Casual_Character_Man.glb",
        "assets/models/Character/Hoodie_Character_Man.glb",
        "assets/models/Character/Farmer_Man.glb",
        "assets/models/Character/Worker_Man.glb",
        "assets/models/Character/Punk_Man.glb",
        "assets/models/Character/Adventurer_Man.glb",
        "assets/models/Character/Business_Man.glb",
        "assets/models/Character/King_Man.glb",
    };
    static const char *WOMAN_PATHS[LEVEL_COUNT] = {
        "assets/models/Character/Animated_Woman_2.glb",
        "assets/models/Character/Animated_Woman.glb",
        "assets/models/Character/Worker_Woman.glb",
        "assets/models/Character/Soldier_Woman.glb",
        "assets/models/Character/Punk_Woman.glb",
        "assets/models/Character/Adventurer_Woman.glb",
        "assets/models/Character/Suit_Woman.glb",
        "assets/models/Character/Witch_Woman.glb",
    };

    std::clog << "[Asset] Loading character roster (16 models)...\n";
    for (int i = 0; i < LEVEL_COUNT; ++i) {
        loadCharacter(Gender::MAN,   i, MAN_PATHS[i]);
        loadCharacter(Gender::WOMAN, i, WOMAN_PATHS[i]);
    }
}

void ModelPlaceholder::loadCharacter(Gender gender, int levelIndex, const std::string &path)
{
    CharacterAsset &asset = _characters[static_cast<int>(gender)][levelIndex];

    asset.loaded = loadModelFile(asset.model, path);
    if (!asset.loaded) return;

    asset.bounds = GetModelBoundingBox(asset.model);
    applyOrientationFix(asset.model);
    asset.anims  = LoadModelAnimations(path.c_str(), &asset.animCount);
    if (asset.anims == nullptr)
        asset.animCount = 0;
    if (asset.animCount > 0)
        asset.gpuSkinned = GpuDraw::prepareSkinnedModel(asset.model);

    std::clog << "[Anim Debug] " << path << " -> " << asset.animCount
              << " clip(s), model bones=" << asset.model.boneCount << "\n";
    for (int i = 0; i < asset.animCount; ++i) {
        std::clog << "    [" << i << "] \"" << asset.anims[i].name
                  << "\" frames=" << asset.anims[i].frameCount
                  << " bones=" << asset.anims[i].boneCount << "\n";
    }
}

ModelPlaceholder::~ModelPlaceholder()
{
    for (auto &pair : _models) {
        UnloadModel(pair.second);
    }
    if (_playerModelLoaded) {
        UnloadModel(_playerModel);
    }
    if (_gemLoaded) {
        UnloadModel(_gemModel);
    }
    if (_crownLoaded) {
        UnloadModel(_crownModel);
    }
    if (_eggLoaded) {
        UnloadModel(_eggModel);
    }
    for (int g = 0; g < 2; ++g) {
        for (int l = 0; l < LEVEL_COUNT; ++l) {
            CharacterAsset &asset = _characters[g][l];
            if (!asset.loaded) continue;
            if (asset.anims != nullptr)
                UnloadModelAnimations(asset.anims, asset.animCount);
            UnloadModel(asset.model);
        }
    }
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

void ModelPlaceholder::drawResource(Resource type, Vector3 position, float scaleMul) const
{
    auto it = _models.find(type);
    if (it != _models.end()) {
        Color tint = (type == FOOD) ? WHITE : _resourceColors[type];
        float currentScale = _resourceScales[type] * scaleMul;
        DrawModel(it->second, position, currentScale, tint);
    } else {
        float radius = 0.15f * scaleMul;
        DrawSphere(position, radius, _resourceColors[type]);
        DrawSphereWires(position, radius, 4, 4, DARKGRAY);
    }
}

void ModelPlaceholder::drawResources(Resource type, const std::vector<Vector3> &positions)
{
    if (positions.empty())
        return;
    auto it = _models.find(type);
    if (it == _models.end()) {
        for (const Vector3 &p : positions)
            drawResource(type, p);
        return;
    }
    const Model &model = it->second;
    float s = _resourceScales[type];
    _scratch.clear();
    for (const Vector3 &p : positions)
        _scratch.push_back(GpuDraw::modelTransform(model, p, 0.0f, {s, s, s}));
    Color tint = (type == FOOD) ? WHITE : _resourceColors[type];
    GpuDraw::drawModelInstanced(model, _scratch.data(),
                                static_cast<int>(_scratch.size()), tint);
}

const CharacterAsset *ModelPlaceholder::getCharacterAsset(Gender gender, int level) const
{
    int lvl = level;
    if (lvl < 1) lvl = 1;
    if (lvl > LEVEL_COUNT) lvl = LEVEL_COUNT;

    const CharacterAsset &asset = _characters[static_cast<int>(gender)][lvl - 1];
    return asset.loaded ? &asset : nullptr;
}

int ModelPlaceholder::resolveAnimIndex(const CharacterAsset &asset, ClipState state) const
{
    const char *name = "Idle";
    switch (state) {
        case ClipState::Idle:       name = "Idle";         break;
        case ClipState::Walk:       name = "Walk";         break;
        case ClipState::HitAgony:   name = "HitRecieve";   break;
        case ClipState::Death:      name = "Death";        break;
        case ClipState::Roll:       name = "Roll";         break;
        case ClipState::SwordSlash: name = "Sword_Slash";  break;
        case ClipState::Interact:   name = "Interact";     break;
        case ClipState::Wave:       name = "Wave";         break;
        case ClipState::KickLeft:   name = "Kick_Left";    break;
        case ClipState::KickRight:  name = "Kick_Right";   break;
        case ClipState::PunchLeft:  name = "Punch_Left";   break;
        case ClipState::PunchRight: name = "Punch_Right";  break;
        case ClipState::Eject:      name = "HitRecieve_2"; break;
    }

    for (int i = 0; i < asset.animCount; ++i) {
        if (TextIsEqual(asset.anims[i].name, name))
            return i;
    }
    return asset.animCount > 0 ? 0 : -1;
}

bool ModelPlaceholder::isLoopingClip(ClipState state) const
{
    return state == ClipState::Idle || state == ClipState::Walk || state == ClipState::Roll;
}

static float orientationToAngle(int orientation)
{
    if (orientation == 1) return 180.0f;
    if (orientation == 2) return 90.0f;
    if (orientation == 3) return 0.0f;
    if (orientation == 4) return 270.0f;
    return 0.0f;
}

static bool isLockedClip(ClipState state)
{
    switch (state) {
        case ClipState::Idle:
        case ClipState::Walk:
            return false;
        default:
            return true;
    }
}

void ModelPlaceholder::updatePlayers(World &world, float deltaTime)
{
    for (const auto &ev : world.animEvents) {
        PlayerAnimState &st = _animStates[ev.playerId];
        if (st.dead) continue;

        switch (ev.action) {
            case AnimAction::Death:
                st.dead    = true;
                st.current = ClipState::HitAgony;
                st.frame   = 0.0f;
                break;
            case AnimAction::Roll:
                st.current = ClipState::Roll;
                st.frame   = 0.0f;
                break;
            case AnimAction::RollEnd:
                st.current = ClipState::Idle;
                st.frame   = 0.0f;
                break;
            case AnimAction::SwordSlash:
                st.current = ClipState::SwordSlash;
                st.frame   = 0.0f;
                break;
            case AnimAction::Interact:
                st.current = ClipState::Interact;
                st.frame   = 0.0f;
                break;
            case AnimAction::Wave:
                st.current = ClipState::Wave;
                st.frame   = 0.0f;
                break;
            case AnimAction::KickLeft:
                st.current = ClipState::KickLeft;
                st.frame   = 0.0f;
                break;
            case AnimAction::KickRight:
                st.current = ClipState::KickRight;
                st.frame   = 0.0f;
                break;
            case AnimAction::PunchLeft:
                st.current = ClipState::PunchLeft;
                st.frame   = 0.0f;
                break;
            case AnimAction::PunchRight:
                st.current = ClipState::PunchRight;
                st.frame   = 0.0f;
                break;
            case AnimAction::HitReceiveEject:
                st.current = ClipState::Eject;
                st.frame   = 0.0f;
                break;
        }
    }
    world.animEvents.clear();

    for (auto &pair : world.players) {
        Player          &p  = pair.second;
        PlayerAnimState &st = _animStates[p.id];

        if (!st.initialized) {
            st.visX        = static_cast<float>(p.x);
            st.visY        = static_cast<float>(p.y);
            st.visYaw      = orientationToAngle(static_cast<int>(p.orientation));
            st.initialized = true;
        }

        // Rotation progressive vers l'orientation serveur, par le chemin
        // angulaire le plus court (270° -> 0° tourne de +90°, pas de -270°).
        float targetYaw = orientationToAngle(static_cast<int>(p.orientation));
        float dyaw = targetYaw - st.visYaw;
        while (dyaw > 180.0f)  dyaw -= 360.0f;
        while (dyaw < -180.0f) dyaw += 360.0f;
        float maxTurn = TURN_DEG_PER_SEC * deltaTime;
        if (fabsf(dyaw) <= maxTurn)
            st.visYaw = targetYaw;
        else
            st.visYaw += (dyaw > 0.0f) ? maxTurn : -maxTurn;
        st.visYaw = fmodf(st.visYaw + 360.0f, 360.0f);

        // Delta vers la case serveur, par le chemin le plus court sur la
        // carte torique (aller de x=9 à x=0 traverse le bord, pas la carte).
        float dx = static_cast<float>(p.x) - st.visX;
        float dy = static_cast<float>(p.y) - st.visY;
        if (world.width > 0) {
            float w = static_cast<float>(world.width);
            if (dx > w * 0.5f) dx -= w;
            if (dx < -w * 0.5f) dx += w;
        }
        if (world.height > 0) {
            float h = static_cast<float>(world.height);
            if (dy > h * 0.5f) dy -= h;
            if (dy < -h * 0.5f) dy += h;
        }

        float dist   = sqrtf(dx * dx + dy * dy);
        bool  moving = dist > 0.001f;

        if (dist > 3.0f) {
            // Trop de retard accumulé (serveur très rapide) : on se recale.
            st.visX = static_cast<float>(p.x);
            st.visY = static_cast<float>(p.y);
            moving  = false;
        } else if (moving) {
            // Accélère au-delà d'une case de retard pour ne jamais décrocher.
            float speed = WALK_TILES_PER_SEC * (dist > 1.0f ? dist : 1.0f);
            float step  = speed * deltaTime;
            if (step >= dist) {
                st.visX = static_cast<float>(p.x);
                st.visY = static_cast<float>(p.y);
                moving  = false;
            } else {
                st.visX += dx / dist * step;
                st.visY += dy / dist * step;
            }
        }

        // Re-plie la position visuelle dans la carte après un passage de bord.
        if (world.width > 0)
            st.visX = fmodf(st.visX + world.width, static_cast<float>(world.width));
        if (world.height > 0)
            st.visY = fmodf(st.visY + world.height, static_cast<float>(world.height));

        if (!st.dead && !isLockedClip(st.current)) {
            st.current = moving ? ClipState::Walk : ClipState::Idle;
        }

        Gender gender = (p.id % 2 == 0) ? Gender::MAN : Gender::WOMAN;
        const CharacterAsset *asset = getCharacterAsset(gender, p.level);
        if (asset == nullptr) {
            // Pas de modèle animé : le cadavre placeholder attend au sol.
            if (st.dead)
                st.deadGroundTime += deltaTime;
            continue;
        }

        int animIdx = resolveAnimIndex(*asset, st.current);
        if (animIdx < 0) continue;

        int frameCount = asset->anims[animIdx].frameCount;
        float animRate = (st.current == ClipState::Walk) ? WALK_ANIM_SPEED : 1.0f;
        st.frame += deltaTime * 30.0f * animRate;

        if (isLoopingClip(st.current)) {
            if (frameCount > 0)
                st.frame = fmodf(st.frame, static_cast<float>(frameCount));
        } else if (st.frame >= static_cast<float>(frameCount)) {
            if (st.current == ClipState::HitAgony) {
                st.current = ClipState::Death;
                st.frame   = 0.0f;
            } else if (st.dead) {
                // Animation de mort terminée : le corps reste figé au sol.
                st.frame = static_cast<float>(frameCount > 0 ? frameCount - 1 : 0);
                st.deadGroundTime += deltaTime;
            } else {
                st.current = moving ? ClipState::Walk : ClipState::Idle;
                st.frame   = 0.0f;
            }
        }
    }

    // Les corps restés assez longtemps au sol disparaissent de la carte.
    for (auto it = world.players.begin(); it != world.players.end();) {
        auto anim = _animStates.find(it->first);
        if (anim != _animStates.end() && anim->second.dead
            && anim->second.deadGroundTime >= DEATH_LINGER_SECONDS) {
            _animStates.erase(anim);
            it = world.players.erase(it);
        } else {
            ++it;
        }
    }
}

Vector2 ModelPlaceholder::getVisualTilePos(int playerId, int fallbackX, int fallbackY) const
{
    auto it = _animStates.find(playerId);
    if (it != _animStates.end() && it->second.initialized)
        return {it->second.visX, it->second.visY};
    return {static_cast<float>(fallbackX), static_cast<float>(fallbackY)};
}

void ModelPlaceholder::drawPlayer(int playerId, Vector3 position, int orientation, int level,
                                  Color teamColor, bool isQueen)
{
    auto it = _animStates.find(playerId);

    // Cap interpolé par updatePlayers ; l'orientation serveur sert de secours
    // tant que le joueur n'a pas encore d'état d'animation.
    float rotationAngle = orientationToAngle(orientation);
    if (it != _animStates.end() && it->second.initialized)
        rotationAngle = it->second.visYaw;

    Gender gender = (playerId % 2 == 0) ? Gender::MAN : Gender::WOMAN;
    const CharacterAsset *asset = getCharacterAsset(gender, level);

    if (asset != nullptr) {
        ClipState clip  = ClipState::Idle;
        float     frame = 0.0f;
        if (it != _animStates.end()) {
            clip  = it->second.current;
            frame = it->second.frame;
        }

        int animIdx = resolveAnimIndex(*asset, clip);

        Vector3 modelScale = {_playerScale, _playerScale, _playerScale};
        Vector3 drawPos     = position;
        drawPos.y += PLAYER_Y_OFFSET;

        // Le modèle est partagé par tous les joueurs du même skin : on le
        // met dans la pose de ce joueur juste avant de le dessiner. En
        // skinning GPU, seules les matrices d'os changent (envoyées au
        // shader par le dessin) ; sinon raylib recalcule chaque sommet.
        if (animIdx >= 0 && animIdx < asset->animCount) {
            int f = static_cast<int>(frame);
            if (asset->gpuSkinned)
                UpdateModelAnimationBones(asset->model, asset->anims[animIdx], f);
            else
                UpdateModelAnimation(asset->model, asset->anims[animIdx], f);
        }

        // Pas de teinte : le modèle garde ses couleurs/textures propres.
        DrawModelEx(asset->model, drawPos, {0.0f, 1.0f, 0.0f}, rotationAngle, modelScale, WHITE);
    } else if (_playerModelLoaded) {
        Vector3 modelScale = {_playerScale, _playerScale, _playerScale};
        Vector3 drawPos     = position;
        drawPos.y += PLAYER_Y_OFFSET;
        DrawModelEx(_playerModel, drawPos, {0.0f, 1.0f, 0.0f}, rotationAngle, modelScale, WHITE);
    } else {
        Vector3 size = {0.4f, 0.6f, 0.4f};
        DrawCube(position, size.x, size.y, size.z, teamColor);
        DrawCubeWires(position, size.x, size.y, size.z, BLACK);

        Vector3 nosePos = position;
        float rad = (rotationAngle + 90.0f) * DEG2RAD;
        nosePos.x += sinf(rad) * 0.3f;
        nosePos.z += cosf(rad) * 0.3f;
        DrawSphere(nosePos, 0.1f, RAYWHITE);
    }

    // Gemme d'équipe en lévitation au-dessus de la tête (masquée à la mort) ;
    // tout le monde la porte — elle identifie l'équipe — et la reine a en
    // plus la couronne qui flotte juste au-dessus de la sienne.
    // La hauteur suit la taille réelle du modèle du personnage ; le déphasage
    // par id évite que tous les objets flottent en cadence.
    bool dead = (it != _animStates.end() && it->second.dead);
    if (!dead && (_gemLoaded || (isQueen && _crownLoaded))) {
        float baseY = GEM_BASE_HEIGHT * _playerScale;
        if (asset != nullptr)
            baseY = (asset->bounds.max.y - asset->bounds.min.y) * _playerScale
                  + GEM_HEAD_MARGIN;
        float t     = static_cast<float>(GetTime());
        float phase = static_cast<float>(playerId) * 1.3f;
        float bobY  = baseY + sinf(t * GEM_BOB_SPEED + phase) * GEM_BOB_AMPLITUDE;
        float spin  = fmodf(t * GEM_SPIN_DEG_PER_SEC + phase * 57.0f, 360.0f);

        if (_gemLoaded) {
            Vector3 gemPos = position;
            gemPos.y += bobY;
            Vector3 gemScale = {_gemScale, _gemScale, _gemScale};
            Matrix  m = GpuDraw::modelTransform(_gemModel, gemPos, spin, gemScale);
            GemBatch *batch = nullptr;
            for (GemBatch &b : _gemBatches)
                if (ColorIsEqual(b.color, teamColor))
                    batch = &b;
            if (batch == nullptr) {
                _gemBatches.push_back({teamColor, {}});
                batch = &_gemBatches.back();
            }
            batch->transforms.push_back(m);
        }
        if (isQueen && _crownLoaded) {
            Vector3 crownPos = position;
            crownPos.y += bobY + CROWN_ABOVE_GEM_GAP
                        - _crownCenterY * _crownScale;
            Vector3 crownScale = {_crownScale, _crownScale, _crownScale};
            DrawModelEx(_crownModel, crownPos, {0.0f, 1.0f, 0.0f}, spin,
                        crownScale, WHITE);
        }
    }
}

void ModelPlaceholder::flushGems()
{
    // La gemme a un matériau blanc : la teinte donne la couleur d'équipe.
    for (GemBatch &b : _gemBatches) {
        GpuDraw::drawModelInstanced(_gemModel, b.transforms.data(),
                                    static_cast<int>(b.transforms.size()), b.color);
        b.transforms.clear();
    }
}

void ModelPlaceholder::drawEggs(const std::vector<Vector3> &positions)
{
    if (positions.empty())
        return;
    _scratch.clear();
    for (const Vector3 &p : positions)
        _scratch.push_back(GpuDraw::modelTransform(_eggModel, p, 0.0f, {1.0f, 1.0f, 1.0f}));
    GpuDraw::drawModelInstanced(_eggModel, _scratch.data(),
                                static_cast<int>(_scratch.size()), RAYWHITE);
    // Contours à la suite : même mode de tracé, donc un seul lot raylib.
    for (const Vector3 &p : positions)
        DrawSphereWires(p, EGG_RADIUS, 5, 5, LIGHTGRAY);
}

void ModelPlaceholder::setPlayerScale(float scale)
{
    _playerScale = scale;
}

float ModelPlaceholder::getPlayerScale() const
{
    return _playerScale;
}

float ModelPlaceholder::getCharacterHeight() const
{
    const CharacterAsset &ref = _characters[static_cast<int>(Gender::MAN)][0];
    if (ref.loaded)
        return (ref.bounds.max.y - ref.bounds.min.y) * _playerScale;
    if (_playerModelLoaded)
        return (_playerBounds.max.y - _playerBounds.min.y) * _playerScale;
    return 1.8f;
}

const Color *ModelPlaceholder::getResourceColors() const
{
    return _resourceColors;
}
