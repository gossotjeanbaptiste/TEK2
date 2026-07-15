/*
 ** EPITECH PROJECT, 2024
 ** Arcade
 ** File description:
 ** Core orchestrator implementation
 */

#include "Core.hpp"
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <chrono>

namespace fs = std::filesystem;

arc::Core::Core(const std::string& initialGraphicsLib, const std::string& initialGameLib)
    : _graphics(nullptr), _game(nullptr), _graphicsLoader(),
    _currentGraphicsIndex(0), _currentGameIndex(0), _playerName("Player"),
    _gameRunning(true), _initialGameLib(initialGameLib)
{
    std::cerr << "[Core::ctor] Starting with lib: " << initialGraphicsLib << std::endl;
    try {
        std::cerr << "[Core::ctor] Loading graphics libraries" << std::endl;
        loadGraphicsLibraries();
        std::cerr << "[Core::ctor] Loading game libraries" << std::endl;
        loadGameLibraries();
        std::cerr << "[Core::ctor] Loading scores" << std::endl;
        loadScores();
        std::cerr << "[Core::ctor] Loading graphics from: " << initialGraphicsLib << std::endl;
        _loadGraphicsFromPath(initialGraphicsLib);
        std::cerr << "[Core::ctor] Core initialization complete" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[Core::ctor] Failed to initialize: " << e.what() << std::endl;
        throw;
    }
}

arc::Core::~Core()
{
    _unloadGame();
    _unloadGraphics();
}

void arc::Core::loadGraphicsLibraries()
{
    _availableGraphics.clear();

    try {
        for (const auto& entry : fs::directory_iterator("./lib")) {
            if (entry.path().extension() == ".so" && 
                    entry.path().filename().string().find("arcade_") == 0) {
                try {
                    std::cerr << "[loadGraphicsLibraries] Checking: " << entry.path().filename().string() << std::endl;
                    LibLoader tempLoader;
                    tempLoader.changeLib(entry.path().string());
                    tempLoader.loadFunction<IGraphics*(*)()>("createGraphics");
                    _availableGraphics.push_back(entry.path().string());
                    std::cerr << "[loadGraphicsLibraries] Added as graphics lib" << std::endl;
                } catch (const std::exception& ex) {
                    std::cerr << "[loadGraphicsLibraries] Not a graphics library: " << ex.what() << std::endl;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[loadGraphicsLibraries] Warning: " << e.what() << std::endl;
    }
    std::sort(_availableGraphics.begin(), _availableGraphics.end());
}

void arc::Core::loadGameLibraries()
{
    _availableGames.clear();

    try {
        for (const auto& entry : fs::directory_iterator("./lib")) {
            if (entry.path().extension() == ".so" && 
                    entry.path().filename().string().find("arcade_") == 0) {
                try {
                    LibLoader tempLoader;
                    tempLoader.changeLib(entry.path().string());
                    tempLoader.loadFunction<IGame*(*)()>("createGame");
                    _availableGames.push_back(entry.path().string());
                } catch (...) {
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Warning: Could not load game libraries: " << e.what() << std::endl;
    }

    std::sort(_availableGames.begin(), _availableGames.end());
}

void arc::Core::loadScores()
{
    _scores.clear();
}

void arc::Core::saveScores()
{
}

void arc::Core::_unloadGraphics()
{
    if (!_graphics)
        return;

    try {
        _graphics->close();
    } catch (...) {}

    try {
        auto destroyGraphics = _graphicsLoader.loadFunction<void(*)(IGraphics*)>("destroyGraphics");
        if (destroyGraphics)
            destroyGraphics(_graphics);
    } catch (...) {}

    _graphics = nullptr;
}

void arc::Core::_unloadGame()
{
    if (!_game)
        return;

    try {
        auto destroyGame = _gameLoader.loadFunction<void(*)(IGame*)>("destroyGame");
        if (destroyGame)
            destroyGame(_game);
    } catch (...) {}

    _game = nullptr;
}

void arc::Core::_loadGraphicsFromPath(const std::string& libPath)
{
    try {
        LibLoader tempLoader;
        tempLoader.changeLib(libPath);
        auto createGraphics = tempLoader.loadFunction<IGraphics*(*)()>("createGraphics");
        IGraphics* newGraphics = createGraphics();
        if (!newGraphics)
            throw LibLoader::openException(libPath);

        _unloadGraphics();

        newGraphics->init();
        _graphicsLoader.changeLib(libPath);
        _graphics = newGraphics;
        for (size_t i = 0; i < _availableGraphics.size(); ++i) {
            if (_availableGraphics[i] == libPath) {
                _currentGraphicsIndex = i;
                break;
            }
        }
    } catch (const LibLoader::exception& e) {
        std::cerr << "Error loading graphics library: " << e.what() << std::endl;
        throw;
    }
}

void arc::Core::_loadGameFromPath(const std::string& gamePath)
{
    try {
        LibLoader tempLoader;
        tempLoader.changeLib(gamePath);
        auto createGame = tempLoader.loadFunction<IGame*(*)()>("createGame");
        IGame* newGame = createGame();
        if (!newGame)
            throw LibLoader::openException(gamePath);
        newGame->setPlayerName(_playerName);
        newGame->init();
        _unloadGame();
        _gameLoader.changeLib(gamePath);
        _game = newGame;
        for (size_t i = 0; i < _availableGames.size(); ++i) {
            if (_availableGames[i] == gamePath) {
                _currentGameIndex = i;
                break;
            }
        }
    } catch (const LibLoader::exception& e) {
        std::cerr << "Error loading game: " << e.what() << std::endl;
        throw;
    }
}

bool arc::Core::_validateMenuPreconditions()
{
    if (_availableGames.empty()) {
        std::cerr << "Error: No games available" << std::endl;
        return false;
    }
    if (!_graphics) {
        std::cerr << "Error: Graphics not initialized" << std::endl;
        return false;
    }
    return true;
}

bool arc::Core::_menuLoop()
{
    if (!_validateMenuPreconditions())
        return false;
    try {
        LibLoader menuLoader;
        menuLoader.changeLib("./lib/arcade_menu.so");

        auto createGame = menuLoader.loadFunction<IGame*(*)()>("createGame");
        IGame* menuGame = createGame();
        if (!menuGame)
            throw std::runtime_error("Failed to create menu");

        menuGame->init();

        auto lastTime = std::chrono::high_resolution_clock::now();

        while (menuGame->isRunning() && _gameRunning) {
            auto now = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float>(now - lastTime).count();
            lastTime = now;

            Event event = _graphics->getInput();
            menuGame->handleInput(event);
            menuGame->update(deltaTime);
            menuGame->render(_graphics);
        }

        std::string selectedGame = menuGame->getSelectedGame();
        std::string selectedGraphics = menuGame->getSelectedGraphics();
        bool hasSelection = menuGame->hasSelectedGame();

        try {
            auto destroyGame = menuLoader.loadFunction<void(*)(IGame*)>("destroyGame");
            if (destroyGame)
                destroyGame(menuGame);
        } catch (...) {}

        if (!hasSelection || selectedGame.empty())
            return false;

        try {
            _playerName = "Player";
            _loadGraphicsFromPath(selectedGraphics);
            _loadGameFromPath(selectedGame);
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error loading selected game/graphics: " << e.what() << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading menu: " << e.what() << std::endl;
        return false;
    }
}

bool arc::Core::_processGameInput(const Event& event)
{
    if (event.type == EventType::KeyPressed) {
        switch (event.key) {
            case 'M':
            case 'm':
                return true;
            case 'Q':
            case 'q':
                _gameRunning = false;
                return true;
            case 'N':
            case 'n':
                _switchGraphicsLibraryNext();
                break;
            default:
                if (_game)
                    _game->handleInput(event);
        }
    } else if (event.type == EventType::Quit) {
        _gameRunning = false;
        return true;
    } else {
        if (_game)
            _game->handleInput(event);
    }
    return false;
}

void arc::Core::_updateAndRenderGame(float deltaTime)
{
    if (!_game || !_graphics)
        return;

    _game->update(deltaTime);
    _graphics->clear();
    _game->render(_graphics);
    _graphics->refresh();
}

void arc::Core::_switchGraphicsLibraryNext()
{
    if (_availableGraphics.empty())
        return;

    int nextIndex = (_currentGraphicsIndex + 1) % _availableGraphics.size();
    try {
        _loadGraphicsFromPath(_availableGraphics[nextIndex]);
    } catch (...) {
        std::cerr << "Failed to switch graphics library" << std::endl;
    }
}

void arc::Core::_handleGameEnd()
{
    if (_game && !_game->isRunning()) {
        _scores[_game->getPlayerName()] = _game->getScore();
        saveScores();
    }
}

void arc::Core::_gameLoop()
{
    if (!_game || !_graphics) {
        std::cerr << "Error: Game or graphics not initialized" << std::endl;
        return;
    }
    auto lastTime = std::chrono::high_resolution_clock::now();
    bool shouldExitGame = false;

    while (_game && _gameRunning && !shouldExitGame) {
        auto now = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;
        Event event = _graphics->getInput();
        shouldExitGame = _processGameInput(event);
        _updateAndRenderGame(deltaTime);
    }
    _handleGameEnd();
}

void arc::Core::run()
{
    if (!_initialGameLib.empty()) {
        try {
            _loadGameFromPath(_initialGameLib);
        } catch (const std::exception& e) {
            std::cerr << "Error loading initial game: " << e.what() << std::endl;
            return;
        }
    }

    while (_gameRunning) {
        if (!_game) {
            if (!_menuLoop()) { break; }
        }
        _gameLoop();
        _unloadGame();
    }
}

void arc::Core::quit()
{
    _gameRunning = false;
}
