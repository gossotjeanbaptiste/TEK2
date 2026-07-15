/*
** EPITECH PROJECT, 2024
** Arcade
** File description:
** Menu game implementation
*/

#include <Menu.hpp>
#include <IGraphics.hpp>
#include <IEvents.hpp>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;


arc::Menu::Menu(const std::vector<std::string>& games, 
            const std::vector<std::string>& graphics)
    : _availableGames(games),
        _availableGraphics(graphics),
        _mode(MenuMode::GameSelection),
        _gameIndex(0),
        _graphicsIndex(0),
        _playerName("Player"),
        _gameRunning(true),
        _gameSelected(false)
{
    if (games.empty() || graphics.empty()) {
        loadAvailableLibraries();
    }
}

void arc::Menu::loadAvailableLibraries()
{
    if (_availableGames.empty()) {
        try {
            for (const auto& entry : fs::directory_iterator("./lib")) {
                if (entry.path().extension() == ".so" && 
                    entry.path().filename().string().find("arcade_") == 0) {
                    std::string filename = entry.path().filename().string();
                    if (filename.find("ncurses") == std::string::npos &&
                        filename.find("raylib") == std::string::npos &&
                        filename.find("sdl2") == std::string::npos) {
                        _availableGames.push_back(entry.path().string());
                    }
                }
            }
        } catch (...) {}
        std::sort(_availableGames.begin(), _availableGames.end());
    }
    
    if (_availableGraphics.empty()) {
        try {
            for (const auto& entry : fs::directory_iterator("./lib")) {
                if (entry.path().extension() == ".so" && 
                    entry.path().filename().string().find("arcade_") == 0) {
                    std::string filename = entry.path().filename().string();
                    if (filename.find("ncurses") != std::string::npos ||
                        filename.find("raylib") != std::string::npos ||
                        filename.find("sdl2") != std::string::npos) {
                        _availableGraphics.push_back(entry.path().string());
                    }
                }
            }
        } catch (...) {}
        std::sort(_availableGraphics.begin(), _availableGraphics.end());
    }
}

void arc::Menu::init()
{}

void arc::Menu::update(float deltaTime)
{
    (void)deltaTime;
}

void arc::Menu::_renderTitle(IGraphics* graphics, int& y)
{
    graphics->drawString(5, y, "=== ARCADE LAUNCHER ===", 0);
    y += 2;
}

void arc::Menu::_renderGameList(IGraphics* graphics, int& y)
{
    graphics->drawString(2, y, "SELECT GAME:", 0);
    y += 1;
    
    for (size_t i = 0; i < _availableGames.size(); ++i) {
        std::string prefix = (i == (size_t)_gameIndex) ? "> " : "  ";
        std::string gameName = extractName(_availableGames[i]);
        graphics->drawString(4, y, prefix + gameName, 0);
        y += 1;
    }
}

void arc::Menu::_renderGraphicsList(IGraphics* graphics, int& y)
{
    y += 1;
    graphics->drawString(2, y, "AVAILABLE GRAPHICS:", 0);
    y += 1;
    
    for (size_t i = 0; i < _availableGraphics.size(); ++i) {
        std::string prefix = (i == (size_t)_graphicsIndex) ? "[*] " : "[ ] ";
        std::string libName = extractName(_availableGraphics[i]);
        graphics->drawString(4, y, prefix + libName, 0);
        y += 1;
    }
}

void arc::Menu::_renderControls(IGraphics* graphics, int& y)
{
    y += 2;
    graphics->drawString(2, y, "CONTROLS:", 0);
    y += 1;
    graphics->drawString(4, y, "UP/DOWN - Navigate games", 0);
    y += 1;
    graphics->drawString(4, y, "LEFT/RIGHT - Change graphics", 0);
    y += 1;
    graphics->drawString(4, y, "ENTER - Start selected game", 0);
    y += 1;
    graphics->drawString(4, y, "ESC - Quit", 0);
}

void arc::Menu::render(IGraphics* graphics)
{
    graphics->clear();
    int y = 0;
    
    _renderTitle(graphics, y);
    
    if (_mode == MenuMode::GameSelection) {
        _renderGameList(graphics, y);
        _renderGraphicsList(graphics, y);
        _renderControls(graphics, y);
    }
    
    graphics->refresh();
}

void arc::Menu::handleInput(const Event& event)
{
    if (event.type == EventType::Quit) {
        _gameRunning = false;
        return;
    }
    if (event.type == EventType::KeyPressed) {
        if (event.key == 256) { 
            _gameRunning = false;
            return;
        } else if ((event.key == 'w' || event.key == 'W' || event.key == 259)
        && _mode == MenuMode::GameSelection) {
            _gameIndex = (_gameIndex - 1 + _availableGames.size()) % _availableGames.size();
        } else if ((event.key == 's' || event.key == 'S' || event.key == 258)
        && _mode == MenuMode::GameSelection) {
            _gameIndex = (_gameIndex + 1) % _availableGames.size();
        } else if ((event.key == 'a' || event.key == 'A' || event.key == 260)
        && _mode == MenuMode::GameSelection) {
            _graphicsIndex = (_graphicsIndex - 1 + _availableGraphics.size()) % _availableGraphics.size();
        } else if ((event.key == 'd' || event.key == 'D' || event.key == 261)
        && _mode == MenuMode::GameSelection) {
            _graphicsIndex = (_graphicsIndex + 1) % _availableGraphics.size();
        } else if ((event.key == '\n' || event.key == 10 || event.key == 13)
        && _mode == MenuMode::GameSelection) {
            _gameSelected = true;
            _gameRunning = false;
        }
    }
}

bool arc::Menu::isRunning() const
{
    return _gameRunning;
}

void arc::Menu::reset()
{
    _gameIndex = 0;
    _graphicsIndex = 0;
    _gameRunning = true;
    _gameSelected = false;
    _mode = MenuMode::GameSelection;
}

std::string arc::Menu::getSelectedGame() const
{
    if (_gameIndex < (int)_availableGames.size())
        return _availableGames[_gameIndex];
    return "";
}

std::string arc::Menu::getSelectedGraphics() const
{
    if (_graphicsIndex < (int)_availableGraphics.size())
        return _availableGraphics[_graphicsIndex];
    return "";
}

std::string arc::Menu::extractName(const std::string& path) const
{
    fs::path p(path);
    std::string filename = p.stem().string();
    if (filename.find("arcade_") == 0)
        filename = filename.substr(7);
    if (!filename.empty()) 
        filename[0] = std::toupper(filename[0]);
    return filename;
}

extern "C" {
    arc::IGame* createGame()
    {
        return new arc::Menu(std::vector<std::string>(), std::vector<std::string>());
    }

    void destroyGame(arc::IGame* game)
    {
        delete game;
    }
}
