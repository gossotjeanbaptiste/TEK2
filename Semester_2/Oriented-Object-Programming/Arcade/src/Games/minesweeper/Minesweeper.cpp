/*
** EPITECH PROJECT, 2024
** Arcade
** File description:
** Minesweeper game implementation
*/

#include "Minesweeper.hpp"
#include <cmath>
#include <iostream>

arc::Minesweeper::Minesweeper() :
    cursorX(0), cursorY(0), gameOver(false), 
    won(false), running(true), score(0), 
    cellsRevealed(0), playerName("Player"),
    lastKeyCode(0), flagMode(false)
{
    grid.resize(GRID_HEIGHT, std::vector<Cell>(GRID_WIDTH));
}

void arc::Minesweeper::initializeGrid()
{
    for (int y = 0; y < GRID_HEIGHT; y++)
        for (int x = 0; x < GRID_WIDTH; x++)
            grid[y][x] = Cell();
    cellsRevealed = 0;
    gameOver = false;
    won = false;
    score = 0;
    flagMode = false;
    cursorX = 0;
    cursorY = 0;
    placeMines();
    calculateAdjacentMines();
}

void arc::Minesweeper::placeMines()
{
    int minesPlaced = 0;
    while (minesPlaced < MINES_COUNT) {
        int x = rand() % GRID_WIDTH;
        int y = rand() % GRID_HEIGHT;
        if (!grid[y][x].isMine) {
            grid[y][x].isMine = true;
            minesPlaced++;
        }
    }
}

int arc::Minesweeper::_countAdjacentCells(int x, int y, CellCheckType checkType) const
{
    int count = 0;
    for (int dy = -1; dy <= 1; dy++)
        for (int dx = -1; dx <= 1; dx++) {
            int nx = x + dx;
            int ny = y + dy;
            if (!isValidCell(nx, ny)) continue;
            if (checkType == CHECK_MINES && grid[ny][nx].isMine)
                count++;
            else if (checkType == CHECK_FLAGS && grid[ny][nx].isFlagged)
                count++;
        }
    return count;
}

int arc::Minesweeper::countAdjacentMines(int x, int y) const
{
    return _countAdjacentCells(x, y, CHECK_MINES);
}

int arc::Minesweeper::countFlagsAround(int x, int y) const
{
    return _countAdjacentCells(x, y, CHECK_FLAGS);
}

void arc::Minesweeper::calculateAdjacentMines()
{
    for (int y = 0; y < GRID_HEIGHT; y++)
        for (int x = 0; x < GRID_WIDTH; x++)
            if (!grid[y][x].isMine)
                grid[y][x].adjacentMines = countAdjacentMines(x, y);
}

bool arc::Minesweeper::isValidCell(int x, int y) const
{
    return x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT;
}

void arc::Minesweeper::revealCell(int x, int y)
{
    if (!isValidCell(x, y) || grid[y][x].isRevealed || grid[y][x].isFlagged) return;
    grid[y][x].isRevealed = true;
    cellsRevealed++;
    if (grid[y][x].isMine) {
        gameOver = true;
        revealAllMines();
        return;
    }
    score += 10;
    if (grid[y][x].adjacentMines == 0)
        for (int dy = -1; dy <= 1; dy++)
            for (int dx = -1; dx <= 1; dx++)
                revealCell(x + dx, y + dy);
    checkWinCondition();
}

void arc::Minesweeper::toggleFlag(int x, int y)
{
    if (!isValidCell(x, y) || grid[y][x].isRevealed) return;
    grid[y][x].isFlagged = !grid[y][x].isFlagged;
    if (grid[y][x].isFlagged)
        score += 5;
    else
        score -= 5;
}

void arc::Minesweeper::revealAllMines()
{
    for (int y = 0; y < GRID_HEIGHT; y++)
        for (int x = 0; x < GRID_WIDTH; x++)
            if (grid[y][x].isMine)
                grid[y][x].isRevealed = true;
}

void arc::Minesweeper::checkWinCondition()
{
    int totalCells = GRID_WIDTH * GRID_HEIGHT;
    if (cellsRevealed == totalCells - MINES_COUNT) {
        won = true;
        running = false;
        score += 500;
    }
}

void arc::Minesweeper::revealAroundCell(int x, int y)
{
    // Only works on revealed cells with numbers
    if (!isValidCell(x, y) || !grid[y][x].isRevealed || grid[y][x].isMine) return;
    int adjacentMines = grid[y][x].adjacentMines;
    int flagsAround = countFlagsAround(x, y);
    // Only proceed if number of flags equals number of adjacent mines
    if (flagsAround != adjacentMines) return;
    // Reveal all non-flagged cells around this cell
    for (int dy = -1; dy <= 1; dy++)
        for (int dx = -1; dx <= 1; dx++) {
            int nx = x + dx;
            int ny = y + dy;
            if (isValidCell(nx, ny) && !grid[ny][nx].isRevealed && !grid[ny][nx].isFlagged)
                revealCell(nx, ny);
        }
}

void arc::Minesweeper::init()
{
    srand(time(nullptr));
    initializeGrid();
}

void arc::Minesweeper::update(float deltaTime)
{
    (void)deltaTime;
    // Game logic handled in handleInput
}

void arc::Minesweeper::render(IGraphics* graphics)
{
    if (!graphics) return;
    graphics->clear();
    _renderHeader(graphics);
    _renderStatus(graphics);
    _renderModeInfo(graphics);
    _renderGrid(graphics);
    graphics->refresh();
}

void arc::Minesweeper::_renderHeader(IGraphics* graphics)
{
    graphics->drawString(2, 0, "MINESWEEPER", 0);
    graphics->drawString(2, 1, "Name: " + playerName, 0);
    graphics->drawString(22, 1, "Score: " + std::to_string(score), 0);
}

void arc::Minesweeper::_renderStatus(IGraphics* graphics)
{
    if (gameOver)
        graphics->drawString(2, 2, "GAME OVER! You hit a mine!", 1);
    else if (won)
        graphics->drawString(2, 2, "YOU WON! Congratulations!", 2);
    else
        graphics->drawString(2, 2, "Click to act | F to toggle mode | 0-9 for chord | R=Reset | ESC=Quit", 0);
}

void arc::Minesweeper::_renderModeInfo(IGraphics* graphics)
{
    std::string modeStr = flagMode ? "Mode: FLAG (click to place)" : "Mode: REVEAL (click to uncover)";
    graphics->drawString(2, 3, modeStr, 0);
}

void arc::Minesweeper::_renderGrid(IGraphics* graphics)
{
    int startX = 2;
    int startY = 4;

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            int screenX = startX + x * 2;
            int screenY = startY + y;
            
            if (grid[y][x].isRevealed)
                if (grid[y][x].isMine)
                    graphics->drawString(screenX, screenY, "* ", 1);
                else if (grid[y][x].adjacentMines > 0)
                    graphics->drawString(screenX, screenY, std::to_string(grid[y][x].adjacentMines) + " ", 0);
                else
                    graphics->drawString(screenX, screenY, ". ", 0);
            else if (grid[y][x].isFlagged)
                graphics->drawString(screenX, screenY, "F ", 2);
            else
                graphics->drawString(screenX, screenY, "? ", 0);
        }
    }
}

void arc::Minesweeper::handleInput(const arc::Event& event)
{
    if (event.type == EventType::KeyPressed)
        _handleKeyInput(event.key);
    else if (event.type == EventType::MousePressed)
        _handleMouseInput(event.mouseX, event.mouseY);
    else if (event.type == EventType::Quit)
        running = false;
}

void arc::Minesweeper::_handleKeyInput(int key)
{
    lastKeyCode = key;
    if (key == 70 || key == 102) { // F or f - toggle mode
        flagMode = !flagMode;
    } else if (key >= 48 && key <= 57) { // 0-9
        // Check if clicked cell has a number matching the digit
        if (!gameOver && !won) {
            int digit = key - 48;
            if (grid[cursorY][cursorX].isRevealed && 
                !grid[cursorY][cursorX].isMine && 
                grid[cursorY][cursorX].adjacentMines == digit) {
                revealAroundCell(cursorX, cursorY);
            }
        }
    } else if (key == 82 || key == 114) { // R or r
        initializeGrid();
    } else if (key == 27 || key == 256) { // ESCAPE (27 = old ncurses code, 256 = new standard)
        running = false;
    }
}

void arc::Minesweeper::_handleMouseInput(int mouseX, int mouseY)
{
    // Grid starts at (2, 4) with cells spaced 2 characters apart
    int startX = 2;
    int startY = 4;
    // Calculate grid position from mouse position
    int gridX = (mouseX - startX) / 2;
    int gridY = mouseY - startY;
    // Check if click is within grid bounds
    if (gridX >= 0 && gridX < GRID_WIDTH && gridY >= 0 && gridY < GRID_HEIGHT) {
        // Move cursor to clicked cell
        cursorX = gridX;
        cursorY = gridY;
        // Perform action based on mode
        if (!gameOver && !won) {
            // If clicking on a revealed cell with flags around, check for chord
            if (grid[cursorY][cursorX].isRevealed && !grid[cursorY][cursorX].isMine) {
                int flagsAround = countFlagsAround(cursorX, cursorY);
                if (flagsAround == grid[cursorY][cursorX].adjacentMines) {
                    revealAroundCell(cursorX, cursorY);
                    return;
                }
            }
            if (flagMode)
                toggleFlag(cursorX, cursorY);
            else
                revealCell(cursorX, cursorY);
        }
    }
}

bool arc::Minesweeper::isRunning() const
{
    return running;
}

int arc::Minesweeper::getScore() const
{
    return score;
}

std::string arc::Minesweeper::getPlayerName() const
{
    return playerName;
}

void arc::Minesweeper::setPlayerName(const std::string& name)
{
    playerName = name;
}

void arc::Minesweeper::reset()
{
    initializeGrid();
}

std::string arc::Minesweeper::getName() const
{
    return "Minesweeper";
}

extern "C" {
    arc::IGame* createGame()
    {
        return new arc::Minesweeper();
    }

    void destroyGame(arc::IGame* game)
    {
        delete game;
    }
}
