/*
 ** EPITECH PROJECT, 2024
 ** Arcade
 ** File description:
 ** Snake game header
 */

#pragma once

#include "../../../include/IGame.hpp"
#include "../../../include/IGraphics.hpp"
#include <vector>
#include <deque>
#include <string>

namespace arc {

    class Snake : public IGame {
        private:
            static const int GRID_WIDTH = 15;
            static const int GRID_HEIGHT = 15;

            struct Position {
                int x, y;
                Position(int x = 0, int y = 0) : x(x), y(y) {}
                bool operator==(const Position& other) const {
                    return x == other.x && y == other.y;
                }
            };

            enum class Direction {
                UP, DOWN, LEFT, RIGHT, NONE
            };

            enum class CellType {
                EMPTY,
                SNAKE_BODY,
                SNAKE_HEAD,
                FOOD
            };

            std::vector<std::vector<CellType>> grid;
            std::deque<Position> snake;
            Position food;
            Direction currentDirection;
            Direction nextDirection;
            bool gameOver;
            bool won;
            bool running;
            int score;
            int frameCounter;
            int moveDelay;
            std::string playerName;

            void initializeGrid();
            void spawnFood();
            bool isValidPosition(int x, int y) const;
            Position _calculateNewHead() const;
            bool _isNewHeadValid(const Position& newHead) const;
            bool _checkIfAteFood(const Position& newHead) const;
            void _clearSnakeFromGrid();
            void _updateSnakePosition(const Position& newHead, bool ateFood);
            void _drawSnakeOnGrid();
            void moveSnake();
            void checkCollisions();
            void checkWinCondition();
            void _handleKeyInput(int key);
            void _renderHeader(IGraphics* graphics);
            void _renderStatus(IGraphics* graphics);
            void _renderGrid(IGraphics* graphics);

        public:
            Snake();
            ~Snake() = default;

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
