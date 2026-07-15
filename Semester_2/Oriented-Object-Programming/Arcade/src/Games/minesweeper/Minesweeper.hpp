/*
** EPITECH PROJECT, 2024
** Arcade
** File description:
** Minesweeper game implementation
*/

#pragma once

#include "../../../include/IGame.hpp"
#include "../../../include/IGraphics.hpp"
#include <vector>
#include <cstdlib>
#include <ctime>

namespace arc {

    class Minesweeper : public IGame {
        private:
            static const int GRID_WIDTH = 10;
            static const int GRID_HEIGHT = 10;
            static const int MINES_COUNT = 10;

            struct Cell {
                bool isMine = false;
                bool isRevealed = false;
                bool isFlagged = false;
                int adjacentMines = 0;
            };

            std::vector<std::vector<Cell>> grid;
            int cursorX = 0;
            int cursorY = 0;
            bool gameOver = false;
            bool won = false;
            bool running = true;
            int score = 0;
            int cellsRevealed = 0;
            std::string playerName = "Player";
            int lastKeyCode = 0;
            bool flagMode = false; // false = reveal mode, true = flag mode

            enum CellCheckType {
                CHECK_MINES,
                CHECK_FLAGS
            };

            void initializeGrid();
            void placeMines();
            void calculateAdjacentMines();
            void revealCell(int x, int y);
            void toggleFlag(int x, int y);
            void revealAllMines();
            void checkWinCondition();
            int countAdjacentMines(int x, int y) const;
            int countFlagsAround(int x, int y) const;
            int _countAdjacentCells(int x, int y, CellCheckType checkType) const;
            void revealAroundCell(int x, int y);
            bool isValidCell(int x, int y) const;
            void _handleKeyInput(int key);
            void _handleMouseInput(int mouseX, int mouseY);
            void _renderHeader(IGraphics* graphics);
            void _renderStatus(IGraphics* graphics);
            void _renderModeInfo(IGraphics* graphics);
            void _renderGrid(IGraphics* graphics);

        public:
            Minesweeper();
            ~Minesweeper() = default;

            void init() override;
            void update(float deltaTime) override;
            void render(IGraphics* graphics) override;
            void handleInput(const Event& event) override;
            bool isRunning() const override;
            int getScore() const override;
            std::string getPlayerName() const override;
            void setPlayerName(const std::string& name) override;
            void reset() override;
            std::string getName() const override;
    };

}
