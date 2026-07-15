/*
** EPITECH PROJECT, 2024
** Arcade
** File description:
** Snake game implementation
*/

#include "Snake.hpp"
#include <IEvents.hpp>
#include <KeyCodes.hpp>
#include <cstdlib>
#include <ctime>
#include <algorithm>

arc::Snake::Snake() :
    gameOver(false), won(false), running(true), score(0),
    frameCounter(0), moveDelay(10), playerName("Player")
{
    grid.resize(GRID_HEIGHT, std::vector<CellType>(GRID_WIDTH, CellType::EMPTY));
}

void arc::Snake::initializeGrid()
{
    for (int y = 0; y < GRID_HEIGHT; y++)
        for (int x = 0; x < GRID_WIDTH; x++)
            grid[y][x] = CellType::EMPTY;

    snake.clear();
    snake.push_back(Position(GRID_WIDTH / 2, GRID_HEIGHT / 2));
    snake.push_back(Position(GRID_WIDTH / 2 - 1, GRID_HEIGHT / 2));
    snake.push_back(Position(GRID_WIDTH / 2 - 2, GRID_HEIGHT / 2));

    for (const auto& pos : snake)
        grid[pos.y][pos.x] = CellType::SNAKE_BODY;
    grid[snake[0].y][snake[0].x] = CellType::SNAKE_HEAD;

    currentDirection = Direction::RIGHT;
    nextDirection = Direction::RIGHT;
    gameOver = false;
    won = false;
    score = 0;
    frameCounter = 0;

    spawnFood();
}

void arc::Snake::spawnFood()
{
    std::vector<Position> emptyCells;
    for (int y = 0; y < GRID_HEIGHT; y++)
        for (int x = 0; x < GRID_WIDTH; x++)
            if (grid[y][x] == CellType::EMPTY)
                emptyCells.push_back(Position(x, y));
    if (emptyCells.empty()) {
        won = true;
        running = false;
        return;
    }
    int index = rand() % emptyCells.size();
    food = emptyCells[index];
    grid[food.y][food.x] = CellType::FOOD;
}

bool arc::Snake::isValidPosition(int x, int y) const
{
    return x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT;
}

arc::Snake::Position arc::Snake::_calculateNewHead() const
{
    Position newHead = snake[0];
    switch (currentDirection) {
        case Direction::UP:    newHead.y--; break;
        case Direction::DOWN:  newHead.y++; break;
        case Direction::LEFT:  newHead.x--; break;
        case Direction::RIGHT: newHead.x++; break;
        default: break;
    }
    return newHead;
}

bool arc::Snake::_isNewHeadValid(const Position& newHead) const
{
    return isValidPosition(newHead.x, newHead.y);
}

bool arc::Snake::_checkIfAteFood(const Position& newHead) const
{
    return newHead.x == food.x && newHead.y == food.y;
}

void arc::Snake::_clearSnakeFromGrid()
{
    for (const auto& pos : snake)
        if (isValidPosition(pos.x, pos.y))
            grid[pos.y][pos.x] = CellType::EMPTY;
}

void arc::Snake::_updateSnakePosition(const Position& newHead, bool ateFood)
{
    snake.push_front(newHead);
    if (!ateFood)
        snake.pop_back();
    else {
        score += 10;
        spawnFood();
    }
}

void arc::Snake::_drawSnakeOnGrid()
{
    for (const auto& pos : snake)
        if (isValidPosition(pos.x, pos.y))
            grid[pos.y][pos.x] = CellType::SNAKE_BODY;
    grid[snake[0].y][snake[0].x] = CellType::SNAKE_HEAD;
}

void arc::Snake::moveSnake()
{
    currentDirection = nextDirection;
    Position newHead = _calculateNewHead();
    if (!_isNewHeadValid(newHead)) {
        gameOver = true;
        running = false;
        return;
    }
    bool ateFood = _checkIfAteFood(newHead);
    _clearSnakeFromGrid();
    _updateSnakePosition(newHead, ateFood);
    _drawSnakeOnGrid();
    checkCollisions();
    checkWinCondition();
}

void arc::Snake::checkCollisions()
{
    Position head = snake[0];
    if (!isValidPosition(head.x, head.y)) {
        gameOver = true;
        running = false;
        return;
    }
    for (size_t i = 1; i < snake.size(); i++) {
        if (snake[i].x == head.x && snake[i].y == head.y) {
            gameOver = true;
            running = false;
            return;
        }
    }
}

void arc::Snake::checkWinCondition()
{
    bool allFilled = true;
    for (int y = 0; y < GRID_HEIGHT; y++)
        for (int x = 0; x < GRID_WIDTH; x++)
            if (grid[y][x] == CellType::EMPTY)
                allFilled = false;
    if (allFilled) {
        won = true;
        running = false;
        score += 500;
    }
}

void arc::Snake::init()
{
    srand(time(nullptr));
    initializeGrid();
}

void arc::Snake::update(float deltaTime)
{
    (void)deltaTime;
    if (gameOver || won) return;
    frameCounter++;
    if (frameCounter >= moveDelay) {
        frameCounter = 0;
        moveSnake();
    }
}

void arc::Snake::render(IGraphics* graphics)
{
    if (!graphics) return;
    graphics->clear();
    _renderHeader(graphics);
    _renderStatus(graphics);
    _renderGrid(graphics);
    graphics->refresh();
}

void arc::Snake::_renderHeader(IGraphics* graphics)
{
    graphics->drawString(2, 0, "SNAKE", 0);
    graphics->drawString(2, 1, "Name: " + playerName, 0);
    graphics->drawString(22, 1, "Score: " + std::to_string(score), 0);
    graphics->drawString(2, 2, "Use arrow keys to move", 0);
}

void arc::Snake::_renderStatus(IGraphics* graphics)
{
    if (gameOver)
        graphics->drawString(2, 3, "GAME OVER! Press R to restart", 1);
    else if (won)
        graphics->drawString(2, 3, "YOU WON! Perfect game!", 2);
    else
        graphics->drawString(2, 3, "R=Reset | ESC=Quit", 0);
}

void arc::Snake::_renderGrid(IGraphics* graphics)
{
    int startX = 2;
    int startY = 5;
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            int screenX = startX + x * 2;
            int screenY = startY + y;
            switch (grid[y][x]) {
                case CellType::EMPTY:
                    graphics->drawString(screenX, screenY, ". ", 0);
                    break;
                case CellType::SNAKE_BODY:
                    graphics->drawString(screenX, screenY, "O ", 0);
                    break;
                case CellType::SNAKE_HEAD:
                    graphics->drawString(screenX, screenY, "@ ", 0);
                    break;
                case CellType::FOOD:
                    graphics->drawString(screenX, screenY, "X ", 1);
                    break;
            }
        }
    }
}

void arc::Snake::handleInput(const arc::Event& event)
{
    if (event.type == EventType::KeyPressed)
        _handleKeyInput(event.key);
    else if (event.type == EventType::Quit)
        running = false;
}

void arc::Snake::_handleKeyInput(int key)
{
    if (key == 'r' || key == 'R') {
        reset();
        return;
    }
    
    if (key == static_cast<int>(StandardKeyCode::ESCAPE)) {
        running = false;
        return;
    }
    
    if (gameOver || won) return;
    
    if (key == static_cast<int>(StandardKeyCode::UP)) {
        if (currentDirection != Direction::DOWN)
            nextDirection = Direction::UP;
    }
    else if (key == static_cast<int>(StandardKeyCode::DOWN)) {
        if (currentDirection != Direction::UP)
            nextDirection = Direction::DOWN;
    }
    else if (key == static_cast<int>(StandardKeyCode::RIGHT)) {
        if (currentDirection != Direction::LEFT)
            nextDirection = Direction::RIGHT;
    }
    else if (key == static_cast<int>(StandardKeyCode::LEFT)) {
        if (currentDirection != Direction::RIGHT)
            nextDirection = Direction::LEFT;
    }
}

bool arc::Snake::isRunning() const
{
    return running;
}

int arc::Snake::getScore() const
{
    return score;
}

std::string arc::Snake::getPlayerName() const
{
    return playerName;
}

void arc::Snake::setPlayerName(const std::string& name)
{
    playerName = name;
}

void arc::Snake::reset()
{
    initializeGrid();
    running = true;
    gameOver = false;
    won = false;
    frameCounter = 0;
}

std::string arc::Snake::getName() const
{
    return "Snake";
}

extern "C" {
    arc::IGame* createGame()
    {
        return new arc::Snake();
    }

    void destroyGame(arc::IGame* game)
    {
        delete game;
    }
}
