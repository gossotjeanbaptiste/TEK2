/*
** EPITECH PROJECT, 2026
** ModelPlaceholder.hpp
** File description:
** 3D model loading, character animation state machine and rendering
*/

#pragma once

#include <raylib.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "Tile.hpp"
#include "World.hpp"

enum class Gender {
    MAN  = 0,
    WOMAN = 1
};

enum class ClipState {
    Idle,
    Walk,
    HitAgony,
    Death,
    Roll,
    SwordSlash,
    Interact,
    Wave,
    KickLeft,
    KickRight,
    PunchLeft,
    PunchRight,
    Eject
};

struct CharacterAsset {
    Model           model{};
    ModelAnimation *anims     = nullptr;
    int             animCount = 0;
    BoundingBox     bounds{};
    bool            loaded    = false;
    bool            gpuSkinned = false; // animé par le shader (GpuDraw)
};

struct PlayerAnimState {
    ClipState current        = ClipState::Idle;
    float     frame          = 0.0f;
    float     visX           = 0.0f;
    float     visY           = 0.0f;
    float     visYaw         = 0.0f;
    bool      initialized    = false;
    bool      dead           = false;
    float     deadGroundTime = 0.0f; // temps passé au sol, animation finie
};

class ModelPlaceholder {
public:
    static constexpr int LEVEL_COUNT = 8;

    // Taille du modèle du joueur : modifier cette valeur pour l'agrandir/rétrécir.
    // Le modèle est probablement modélisé en "mètres" (~1.7-1.9 unités de haut),
    // donc une échelle proche de 1.0 est le point de départ logique.
    static constexpr float PLAYER_SCALE = 1.0f;

    // Décalage vertical (au-dessus de la tuile) : augmenter/diminuer si le
    // personnage est enterré dans le sol ou flotte trop haut.
    static constexpr float PLAYER_Y_OFFSET = -0.3f;

    // Correction d'orientation du modèle brut, appliquée une fois au chargement
    // (avant la rotation N/E/S/O par joueur). Certains packs de personnages sont
    // exportés à plat ou avec un axe "up" différent. Ajuste ces angles (en degrés)
    // jusqu'à ce que le personnage se tienne debout et regarde devant lui.
    // TEST: remis à -90 après avoir confirmé qu'à 0 le personnage est couché.
    static constexpr float PLAYER_FIX_ROTATE_X = 0.0f;
    static constexpr float PLAYER_FIX_ROTATE_Y = 0.0f;
    static constexpr float PLAYER_FIX_ROTATE_Z = 0.0f;

    // Le serveur téléporte les joueurs de case en case ; le GUI fait glisser
    // le modèle vers sa nouvelle case à cette vitesse (en cases/seconde) pour
    // laisser le temps à l'animation Walk de se jouer.
    static constexpr float WALK_TILES_PER_SEC = 2.5f;

    // Multiplicateur de vitesse du clip Walk pendant un déplacement : le clip
    // est animé pour une marche lente, le glissement entre cases est plus
    // rapide, donc on accélère les jambes pour limiter l'effet "patinage".
    static constexpr float WALK_ANIM_SPEED = 2.0f;

    // Vitesse de rotation visuelle (degrés/seconde) quand le joueur tourne :
    // le serveur change l'orientation d'un coup, le GUI fait pivoter le
    // modèle progressivement (90° en ~0.3s à 300°/s).
    static constexpr float TURN_DEG_PER_SEC = 300.0f;

    // Gemme d'équipe en lévitation au-dessus de la tête : taille cible
    // (unités monde, le modèle est normalisé via sa bounding box), marge
    // au-dessus du crâne du personnage, amplitude/vitesse du flottement et
    // vitesse de rotation sur elle-même. GEM_BASE_HEIGHT ne sert que de
    // secours quand le modèle du personnage n'est pas chargé.
    static constexpr float GEM_SIZE             = 0.45f;
    static constexpr float GEM_HEAD_MARGIN      = 0.35f;
    static constexpr float GEM_BASE_HEIGHT      = 2.2f;
    static constexpr float GEM_BOB_AMPLITUDE    = 0.12f;
    static constexpr float GEM_BOB_SPEED        = 2.5f;
    static constexpr float GEM_SPIN_DEG_PER_SEC = 120.0f;

    // Couronne de la reine (le doyen vivant de chaque équipe) : flotte juste
    // au-dessus de sa gemme d'équipe (conservée pour identifier son camp).
    // Elle garde les couleurs de son modèle (aucune teinte) et reprend le
    // flottement/rotation de la gemme.
    static constexpr float CROWN_SIZE          = 0.60f;
    static constexpr float CROWN_ABOVE_GEM_GAP = 0.55f;

    // À sa mort, le joueur finit son animation (agonie puis chute) et son
    // corps reste au sol ce temps-là avant de disparaître de la carte.
    static constexpr float DEATH_LINGER_SECONDS = 4.0f;

    static constexpr float EGG_RADIUS = 0.2f;

    ModelPlaceholder();
    ~ModelPlaceholder();

    void loadAll();

    void drawResource(Resource type, Vector3 position, float scaleMul = 1.0f) const;
    // Toutes les ressources d'un type en un appel de dessin par mesh.
    void drawResources(Resource type, const std::vector<Vector3> &positions);
    // Les gemmes d'équipe sont mises en lot : appeler flushGems() après le
    // dernier drawPlayer de la frame.
    void drawPlayer(int playerId, Vector3 position, int orientation, int level,
                    Color teamColor, bool isQueen = false);
    void flushGems();
    void drawEggs(const std::vector<Vector3> &positions);

    void updatePlayers(World &world, float deltaTime);

    // Position affichée du joueur, en cases (flottante : interpolée entre
    // l'ancienne case et la nouvelle pendant un déplacement).
    Vector2 getVisualTilePos(int playerId, int fallbackX, int fallbackY) const;

    void  setPlayerScale(float scale);
    float getPlayerScale() const;

    // Hauteur (unités monde, échelle comprise) du personnage de référence.
    // Sert d'étalon pour dimensionner le décor par rapport aux joueurs.
    float getCharacterHeight() const;

    const Color *getResourceColors() const;

private:
    Color _resourceColors[RESOURCE_COUNT];
    float _resourceScales[RESOURCE_COUNT];
    std::unordered_map<Resource, Model> _models;

    Model       _playerModel;
    bool        _playerModelLoaded;
    float       _playerScale = PLAYER_SCALE;
    BoundingBox _playerBounds;

    Model _gemModel{};
    bool  _gemLoaded = false;
    float _gemScale  = 1.0f;

    // Lots de gemmes de la frame, un par couleur d'équipe.
    struct GemBatch {
        Color               color;
        std::vector<Matrix> transforms;
    };
    std::vector<GemBatch> _gemBatches;
    std::vector<Matrix>   _scratch; // matrices d'instances (évite les allocations)

    Model _eggModel{};
    bool  _eggLoaded = false;

    Model _crownModel{};
    bool  _crownLoaded  = false;
    float _crownScale   = 1.0f;
    float _crownCenterY = 0.0f; // centre vertical du modèle brut (origine libre)

    CharacterAsset _characters[2][LEVEL_COUNT];

    std::unordered_map<int, PlayerAnimState> _animStates;

    bool loadModelFile(Model &model, const std::string &path);
    void loadAsset(Resource type, const std::string &path);
    void loadCharacter(Gender gender, int levelIndex, const std::string &path);

    const CharacterAsset *getCharacterAsset(Gender gender, int level) const;
    int  resolveAnimIndex(const CharacterAsset &asset, ClipState state) const;
    bool isLoopingClip(ClipState state) const;
};
