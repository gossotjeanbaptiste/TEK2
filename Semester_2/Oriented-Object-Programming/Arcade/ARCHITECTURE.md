# Architecture Technique - Arcade Core

## 📋 Vue d'ensemble

L'**Arcade Core** est l'orchestrateur central du projet. Il gère :
- Le chargement dynamique des librairies graphiques et jeux
- La boucle principale d'exécution
- L'interaction utilisateur
- Les transitions entre jeux, menus et librairies graphiques

```
┌─────────────────────────────────────────────┐
│           Main Application                  │
│         (./arcade [graphics] [game])        │
└──────────────┬──────────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────────┐
│            Core Orchestrator                │
│  ├─ LibLoader (dynamique)                   │
│  ├─ IGraphics* (interface abstraite)        │
│  └─ IGame* (interface abstraite)            │
└──────────┬──────────────────────┬───────────┘
           │                      │
    ┌──────▼────────┐    ┌────────▼───────┐
    │ Graphics DLLs │    │   Game DLLs    │
    │ (ncurses, ...)│    │ (Snake, ...)   │
    └───────────────┘    └────────────────┘
```

---

## 🏗️ Architecture du Core

### 1. **Classe Core**

```hpp
class Core {
    private:
        // Pointeurs abstraits (pas de dépendance directe)
        IGraphics* _graphics;
        IGame* _game;
        
        // Gestionnaires de chargement dynamique
        LibLoader _graphicsLoader;
        LibLoader _gameLoader;
        
        // État de l'application
        std::vector<std::string> _availableGraphics;
        std::vector<std::string> _availableGames;
        std::map<std::string, int> _scores;
        
        int _currentGraphicsIndex;
        int _currentGameIndex;
        std::string _playerName;
        bool _gameRunning;
};
```

### 2. **Cycle de vie**

#### Phase 1️⃣ : **Initialisation (Constructor)**

```cpp
arc::Core::Core(const std::string& initialGraphicsLib, const std::string& initialGameLib)
{
    // 1. Scan du répertoire ./lib
    loadGraphicsLibraries();   // Trouve arcade_ncurses.so, arcade_sdl2.so...
    loadGameLibraries();       // Trouve arcade_snake.so, arcade_minesweeper.so...
    
    // 2. Charge la première librairie graphique
    _loadGraphicsFromPath(initialGraphicsLib);
    
    // 3. Charge les scores (optionnel)
    loadScores();
}
```

#### Phase 2️⃣ : **Boucle principale (run)**

```
┌──────────────────────┐
│  Core::run()         │
└──────────────────────┘
         │
         ▼
    ┌─────────────────────────────────────────────┐
    │ if (!_game)                                 │
    │   _menuLoop()      (Affiche menu)           │
    │   └─ Sélection jeu + librairie graphique    │
    └─────────────────────────────────────────────┘
         │
         ▼
    ┌─────────────────────────────────────────────┐
    │ _gameLoop()                                 │
    │ ├─ Récupère événements (Event)              │
    │ ├─ Traite les touches (N, M, Q)             │
    │ ├─ Appelle game->update(deltaTime)          │
    │ └─ Appelle game->render(graphics)           │
    └─────────────────────────────────────────────┘
         │
         ▼ (Loop back)
    ┌─────────────────────────────────────────────┐
    │ Retour menu si 'M' pressé                   │
    │ Quitter si 'Q' pressé                       │
    │ Changer graphics lib si 'N' pressé          │
    └─────────────────────────────────────────────┘
```

---

## 🔄 Flux d'exécution détaillé

### **Démarrage avec jeu pré-sélectionné**

```
./arcade ./lib/arcade_ncurses.so ./lib/arcade_snake.so
    │
    ├─ Core ctor: charge les 2 libs
    │
    └─ Core::run()
        ├─ Charge directement arcade_snake.so (skip menu)
        └─ Boucle jeu
```

### **Démarrage sans jeu (mode menu)**

```
./arcade ./lib/arcade_ncurses.so
    │
    ├─ Core ctor: charge les graphics et games
    │
    └─ Core::run()
        ├─ _menuLoop() (affiche menu)
        │   ├─ Charge arcade_menu.so
        │   ├─ Affiche liste jeux/graphics/scores
        │   └─ Attend sélection
        │
        ├─ Si sélection valide:
        │   ├─ Charge la librairie graphique choisie
        │   └─ Charge le jeu choisi
        │
        └─ _gameLoop() (boucle jeu)
            ├─ Chaque frame:
            │   ├─ Event event = graphics->getInput()
            │   ├─ Traite input avec _processGameInput()
            │   ├─ game->update(deltaTime)
            │   ├─ graphics->clear()
            │   ├─ game->render(graphics)
            │   └─ graphics->refresh()
            │
            └─ Quitte quand game->isRunning() == false
```

---

## 🎯 Interactions clavier pendant le jeu

| Touche | Action                      | Fonction                       |
| ------ | --------------------------- | ------------------------------ |
| **N**  | Changer librairie graphique | `_switchGraphicsLibraryNext()` |
| **M**  | Retour au menu              | `_gameLoop()` → retour et menu |
| **Q**  | Quitter entièrement         | `_gameRunning = false`         |
| Autres | Envoyées au jeu             | `game->handleInput(event)`     |

```cpp
bool arc::Core::_processGameInput(const Event& event)
{
    if (event.type == EventType::KeyPressed) {
        switch (event.key) {
            case 'N':  // Next graphics
                _switchGraphicsLibraryNext();
                break;
            case 'M':  // Menu
                return true;  // Exit game loop
            case 'Q':  // Quit
                _gameRunning = false;
                return true;
            default:
                _game->handleInput(event);  // Autres touches au jeu
        }
    }
    return false;
}
```

---

## 🔌 Chargement dynamique (LibLoader Pattern)

### Pourquoi pas `#include` direct ?

❌ **Approche naïve (INTERDITE par le sujet):**

```cpp
#include <ncurses.h>          // ❌ FORBIDDEN
#include <raylib.h>           // ❌ FORBIDDEN

NcursesGraphics* graphics = new NcursesGraphics();  // Dépendance compilée
```

✅ **Notre approche (CORRECTE):**

```cpp
// Aucun #include de ncurses, raylib, SDL!

// Au runtime:
LibLoader loader;
loader.changeLib("./lib/arcade_ncurses.so");
auto createGraphics = loader.loadFunction<IGraphics*(*)()>("createGraphics");
IGraphics* graphics = createGraphics();  // Crée l'instance à runtime
```

### Avantages de cette approche

| Aspect            | Avant (Include direct)              | Après (Runtime)                    |
| ----------------- | ----------------------------------- | ---------------------------------- |
| **Couplage**      | Fort (libairie doit être installée) | Faible (juste un fichier .so)      |
| **Flexibilité**   | Changer de lib = recompiler         | Changer de lib = modifier argument |
| **Dépendances**   | Complexes (ncurses + raylib + SDL)  | Isolées par DLL                    |
| **Extensibilité** | Ajouter lib = modifier code         | Ajouter lib = déposer .so          |
| `ldd ./arcade`    | Affiche ncurses, raylib, SDL...     | Affiche rien! (Parfait!)           |

---

## 🏗️ Choix architecturaux

### 1. **Interfaces abstraites (IGraphics, IGame)**

**Pourquoi pas utiliser les classes directement ?**

```cpp
// ❌ Mauvais: Couplage fort
#include "NcursesGraphics.hpp"
NcursesGraphics graphics;

// ✅ Bon: Interface abstraite
#include "IGraphics.hpp"
IGraphics* graphics = createGraphics();
```

**Avantages :**
- Permute n'importe quelle implémentation graphique
- Le Core ne connaît pas les détails d'implémentation
- Facilite les tests et futures extensions

### 2. **LibLoader wrapper (RAII)**

**Pourquoi wrapper dlopen/dlclose ?**

```cpp
// ❌ Mauvais: dlopen brut
void* handle = dlopen("./lib/arcade_ncurses.so", RTLD_LAZY);
// ... utilisation ...
dlclose(handle);  // Facile d'oublier!

// ✅ Bon: RAII (Resource Acquisition Is Initialization)
class LibLoader {
private:
    void* _handle;
public:
    ~LibLoader() { if (_handle) dlclose(_handle); }  // Garanti!
};
```

### 3. **Gestion des indices au lieu des pointeurs**

**Pourquoi stocker indices et chemins ?**

```cpp
// ❌ Mauvais: Pointer vers la librairie chargée
IGraphics* _availableGraphics[10];  // Pointeurs fragiles
int _currentGraphicsIndex = 0;

// ✅ Bon: Chemins + indices
std::vector<std::string> _availableGraphics;  // "./lib/arcade_ncurses.so"
int _currentGraphicsIndex = 0;                // Index dans le vecteur
```

**Avantages :**
- Facile de basculer : `_availableGraphics[nextIndex]`
- Persiste à travers les recharges
- Pas de dangling pointers

### 4. **Séparation Menu / Game / Core loops**

**Pourquoi 3 boucles au lieu d'une ?**

```cpp
// ✅ Architecture actuelle
void run() {
    while (_gameRunning) {
        if (!_game) _menuLoop();     // Boucle menu
        _gameLoop();                  // Boucle jeu
    }
}
```

**vs Alternatives :**

```cpp
// ❌ Une seule grande boucle
void run() {
    while (_gameRunning) {
        // ... 200 lignes de code ...
        // État bizarre à gérer
    }
}

// ❌ State machine complexe
enum State { MENU, GAME, PAUSE };
State _state;
switch(_state) { ... }  // Trop de cas à gérer
```

**Avantages de notre approche:**
- Code plus lisible et maintenable
- Chaque boucle a une responsabilité unique
- Transitions nettes (menu → game → menu)

---

## 🔄 Gestion des ressources

### Loading d'une librairie graphique

```cpp
void arc::Core::_loadGraphicsFromPath(const std::string& libPath)
{
    try {
        // 1. Crée un loader temporaire
        LibLoader tempLoader;
        tempLoader.changeLib(libPath);
        
        // 2. Récupère la fonction de création
        auto createGraphics = tempLoader.loadFunction<IGraphics*(*)()>("createGraphics");
        
        // 3. Crée l'instance
        IGraphics* newGraphics = createGraphics();
        
        // 4. Décharge l'ancienne (si elle existe)
        _unloadGraphics();
        
        // 5. Initialise la nouvelle
        newGraphics->init();
        
        // 6. Sauvegarde les références
        _graphicsLoader.changeLib(libPath);
        _graphics = newGraphics;
        
    } catch (const LibLoader::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}
```

### Unloading sécurisé

```cpp
void arc::Core::_unloadGraphics()
{
    if (!_graphics)
        return;
    
    try {
        _graphics->close();  // Nettoyage gracieux
    } catch (...) {}
    
    try {
        // Appelle le destructeur de la DLL
        auto destroyGraphics = _graphicsLoader.loadFunction<void(*)(IGraphics*)>("destroyGraphics");
        if (destroyGraphics)
            destroyGraphics(_graphics);
    } catch (...) {}
    
    _graphics = nullptr;
}
```

**Important:** L'ordre est crucial !
1. Fermer la ressource (close)
2. Détruire l'objet (destroyGraphics)
3. Décharger la DLL (LibLoader destructor)

---

## 📊 Delta Time et FPS

```cpp
auto lastTime = std::chrono::high_resolution_clock::now();

while (_game && _gameRunning) {
    auto now = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float>(now - lastTime).count();
    lastTime = now;
    
    // deltaTime = temps écoulé en secondes depuis la dernière frame
    _game->update(deltaTime);  // Le jeu peut ajuster sa vitesse
}
```

**Avantage:** Le jeu s'adapte automatiquement à la FPS :
- Lent PC (30 FPS) → deltaTime ≈ 0.033s
- Rapide PC (60 FPS) → deltaTime ≈ 0.016s
- Le jeu bouge à la même vitesse réelle

---

## ❌ Erreurs à éviter

### 1. **Charger la librairie graphique APRÈS initialisation**

```cpp
// ❌ MAUVAIS
newGraphics->init();
_graphicsLoader.changeLib(libPath);  // Erreur dans les symboles
```

```cpp
// ✅ BON
_graphicsLoader.changeLib(libPath);
newGraphics->init();  // Les symboles sont résolus
```

### 2. **Ne pas décharger l'ancienne librairie**

```cpp
// ❌ MAUVAIS: Fuite mémoire
_graphics = new_graphics;  // Perte de l'ancien pointeur

// ✅ BON: Nettoyage
_unloadGraphics();
_graphics = newGraphics;
```

### 3. **Oublier les vérifications null**

```cpp
// ❌ MAUVAIS
_game->update(dt);  // Crash si _game == nullptr

// ✅ BON
if (_game && _graphics)
    _game->update(dt);
```

---

## 📈 Évolution future possible

### Ajout d'une pause/reprendre

```cpp
enum GameState { MENU, RUNNING, PAUSED, GAME_OVER };
GameState _gameState;

// Dans _gameLoop
if (event.key == 'P')
    _gameState = PAUSED;

if (_gameState == RUNNING)
    game->update(deltaTime);
```

### Gestion des profils joueur

```cpp
std::map<std::string, PlayerProfile> _profiles;

struct PlayerProfile {
    std::string name;
    std::map<std::string, int> scores;
    std::string preferredGraphics;
};
```

### Sauvegarde/Chargement d'état de jeu

```cpp
void saveGameState();
void loadGameState();
// Permet de reprendre un jeu inachevé
```

---

## 🎓 Concepts clés appliqués

| Concept                       | Utilisation                                      |
| ----------------------------- | ------------------------------------------------ |
| **Design Pattern: Singleton** | LibLoader (une seule instance)                   |
| **Design Pattern: Factory**   | `createGraphics()`, `createGame()`               |
| **Design Pattern: Strategy**  | IGraphics et IGame (stratégies interchangeables) |
| **RAII**                      | LibLoader ferme automatiquement dlopen           |
| **Interfaces abstraites**     | IGraphics, IGame (zéro couplage)                 |
| **Separation of Concerns**    | Menu, Game, Graphics séparés                     |
| **Event-driven**              | Gestion input via Event struct                   |
| **Delta Time**                | Indépendance de la FPS                           |

---

## 🚀 Résumé

L'architecture du Core est construite autour de **3 principes clés** :

1. **Indépendance** : Aucune dépendance à une librairie graphique spécifique
2. **Flexibilité** : Changer de jeu/graphique sans recompiler
3. **Maintenabilité** : Code organisé en responsabilités claires

Cela en fait une plateforme **extensible** et **robuste**, parfaite pour un système de plugins.
