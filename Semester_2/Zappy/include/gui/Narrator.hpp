/*
** EPITECH PROJECT, 2026
** Narrator.hpp
** File description:
** In-window commentary: goal legend, team board, activity counters,
** human-readable event log and victory banner
*/

#pragma once

#include <raylib.h>
#include <deque>
#include <map>
#include <string>
#include <vector>
#include "gui/World.hpp"

// Événement marquant, avec sa case : la caméra du mode vidéo et le
// tutoriel s'en servent pour montrer « où ça se passe ».
struct Spotlight {
    FxType      type;
    int         x      = 0;
    int         y      = 0;
    int         value  = 0;
    int         player = -1;
    std::string team;
    double      time   = 0.0;
    int         seq    = 0;    // numéro croissant (détecter les nouveaux)
};

class Narrator {
public:
    struct Stats {
        int pickups = 0, drops = 0, broadcasts = 0, rituals = 0;
        int ritualsOk = 0, ritualsKo = 0, eggs = 0, hatched = 0;
        int deaths = 0, ejects = 0, joined = 0;
    };

    static constexpr int PANEL_W = 370;

    // À appeler chaque frame AVANT renderer.update (qui consomme les
    // fxEvents) : traduit les événements en journal et compteurs.
    void observe(const World &world);

    // Panneau ancré à droite de la fenêtre (touche H pour le masquer).
    void drawPanel(const World &world);
    // Bannière de fin de partie, centrée.
    void drawVictory(const World &world) const;

    void      toggle() { _visible = !_visible; }
    bool      visible() const { return _visible; }
    Rectangle panelRect() const { return _visible ? _rect : Rectangle{}; }
    bool      containsMouse() const;

    const Stats &stats() const { return _stats; }
    double       elapsed() const;
    double       endTime() const { return _endTime; }
    int          spotlightSeq() const { return _seq; }
    const std::deque<Spotlight> &spotlights() const { return _spots; }
    // Dernier événement de ce type (faux s'il n'y en a pas encore eu).
    bool lastOf(FxType type, Spotlight &out) const;
    // Plus haut niveau atteint par l'équipe, et record toutes équipes.
    int  bestLevel(const std::string &team) const;
    int  bestLevelOverall(std::string *team = nullptr) const;

    static std::string formatDuration(double seconds);

private:
    // Type d'entrée : une éclosion (ebo) et l'arrivée du joueur qui en
    // sort (pnw) ne font qu'une ligne.
    enum class Kind { Other, Spawn, Hatch };

    struct Entry {
        double      time;
        std::string text;
        Color       color;
        Kind        kind;
        std::string team;
    };

    void log(const std::string &text, Color color,
             Kind kind = Kind::Other, const std::string &team = "");
    // Fusionne pnw/ebo d'une même éclosion ; vrai si fusionné.
    bool mergeHatch(Kind incoming, const std::string &team);
    void spot(const FxEvent &ev, const std::string &team, int x, int y);
    std::string teamOf(const World &world, int playerId) const;
    std::string teamOnTile(const World &world, int x, int y, int lvl) const;

    float drawTeams(const World &world, float x, float y, float w);
    float drawCounters(float x, float y, float w);
    void  drawLog(float x, float y, float w, float bottom);

    bool                 _visible  = true;
    Rectangle            _rect     = {};
    bool                 _started  = false;
    double               _start    = 0.0;
    double               _endTime  = -1.0;
    int                  _seq      = 0;
    Stats                _stats;
    std::deque<Entry>    _log;
    std::deque<Spotlight> _spots;
    std::map<std::string, int> _best;      // équipe -> niveau max atteint
    std::map<int, std::string> _knownTeam; // joueur -> équipe (même mort)
};
