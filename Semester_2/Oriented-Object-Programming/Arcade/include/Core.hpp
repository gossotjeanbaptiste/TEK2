/*
 ** EPITECH PROJECT, 2024
 ** Arcade
 ** File description:
 ** Core orchestrator for the arcade platform
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include "IGraphics.hpp"
#include "IGame.hpp"
#include "LibLoader.hpp"

namespace arc {

    class Core {
        private:
            IGraphics* _graphics;
            IGame* _game;
            LibLoader _graphicsLoader;
            LibLoader _gameLoader;

            std::vector<std::string> _availableGraphics;
            std::vector<std::string> _availableGames;
            std::map<std::string, int> _scores;

            int _currentGraphicsIndex;
            int _currentGameIndex;
            std::string _playerName;
            bool _gameRunning;
            std::string _initialGameLib;

            void loadGraphicsLibraries();
            void loadGameLibraries();
            void loadScores();
            void saveScores();

            void _loadGraphicsFromPath(const std::string& libPath);
            void _loadGameFromPath(const std::string& gamePath);
            void _unloadGraphics();
            void _unloadGame();

            bool _menuLoop();
            void _gameLoop();

            bool _validateMenuPreconditions();

            bool _processGameInput(const Event& event);
            void _updateAndRenderGame(float deltaTime);
            void _switchGraphicsLibraryNext();
            void _handleGameEnd();

        public:
            Core(const std::string& initialGraphicsLib, const std::string& initialGameLib = "");
            ~Core();

            void run();
            void quit();
    };

}
