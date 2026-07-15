/*
** EPITECH PROJECT, 2024
** Arcade
** File description:
** Menu game for selecting games and graphics libraries
*/

#pragma once

#include "IGame.hpp"
#include <string>
#include <vector>

namespace arc {

    class Menu : public IGame {
        public:
            Menu(const std::vector<std::string>& games, 
                 const std::vector<std::string>& graphics);
            ~Menu() override = default;

            void init() override;
            void update(float deltaTime) override;
            void render(IGraphics* graphics) override;
            void handleInput(const Event& event) override;
            bool isRunning() const override;
            int getScore() const override { return 0; }
            std::string getPlayerName() const override { return _playerName; }
            void setPlayerName(const std::string& name) override { _playerName = name; }
            void reset() override;
            std::string getName() const override { return "Menu"; }

            // Menu-specific accessors
            std::string getSelectedGame() const override;
            std::string getSelectedGraphics() const override;
            bool hasSelectedGame() const override { return _gameSelected; }
            void returnToMenu() { _gameSelected = false; _gameRunning = true; }

        private:
            enum class MenuMode {
                GameSelection,
                GraphicsSelection,
                PlayerNameInput
            };

            // Game and graphics lists (stores full paths)
            std::vector<std::string> _availableGames;
            std::vector<std::string> _availableGraphics;

            // Menu state
            MenuMode _mode;
            int _gameIndex;
            int _graphicsIndex;
            std::string _playerName;
            bool _gameRunning;
            bool _gameSelected;

            // Helper methods
            std::string extractName(const std::string& path) const;
            void loadAvailableLibraries();
            
            // Render helpers
            void _renderTitle(IGraphics* graphics, int& y);
            void _renderGameList(IGraphics* graphics, int& y);
            void _renderGraphicsList(IGraphics* graphics, int& y);
            void _renderControls(IGraphics* graphics, int& y);
    };
}
