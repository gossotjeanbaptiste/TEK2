#include "Raylib_graphics.hpp"
#include "../../../include/LibLoader.hpp"
#include "../../../include/IGraphics.hpp"
#include "../../../include/IEvents.hpp"
#include "../../../include/KeyCodes.hpp"
#include <stdexcept>
#include <iostream>

const int arc::RaylibGraphics::RAYLIB_BLACK = 0;
const int arc::RaylibGraphics::RAYLIB_WHITE = 0xFFFFFFFF;
const int arc::RaylibGraphics::RAYLIB_RED = 0xFF0000FF;
const int arc::RaylibGraphics::RAYLIB_GREEN = 7895;
const int arc::RaylibGraphics::RAYLIB_BLUE = 0x0000FFFF;

arc::RaylibGraphics::RaylibGraphics(int width, int height) 
    : loader(nullptr), initWindow(nullptr), windowShouldClose(nullptr),
    closeWindow(nullptr), beginDrawing(nullptr), endDrawing(nullptr),
    clearBackground(nullptr), drawText(nullptr), getScreenWidth(nullptr),
    getScreenHeight(nullptr), raylibIsKeyPressed(nullptr), getKeyPressed(nullptr),
    setTargetFPS(nullptr), isMouseButtonPressed(nullptr), getMouseX(nullptr), 
    getMouseY(nullptr), width(width), height(height), initialized(false)
{
    try {
        loader = std::make_unique<LibLoader>("shared_libs/graphic_libraylib.so");
        initWindow = loader->loadFunction<InitWindowFn>("InitWindow");
        windowShouldClose = loader->loadFunction<WindowShouldCloseFn>("WindowShouldClose");
        closeWindow = loader->loadFunction<CloseWindowFn>("CloseWindow");
        beginDrawing = loader->loadFunction<BeginDrawingFn>("BeginDrawing");
        endDrawing = loader->loadFunction<EndDrawingFn>("EndDrawing");
        clearBackground = loader->loadFunction<ClearBackgroundFn>("ClearBackground");
        drawText = loader->loadFunction<DrawTextFn>("DrawText");
        getScreenWidth = loader->loadFunction<GetScreenWidthFn>("GetScreenWidth");
        getScreenHeight = loader->loadFunction<GetScreenHeightFn>("GetScreenHeight");
        raylibIsKeyPressed = loader->loadFunction<IsKeyPressedFn>("IsKeyPressed");
        getKeyPressed = loader->loadFunction<GetKeyPressedFn>("GetKeyPressed");
        setTargetFPS = loader->loadFunction<SetTargetFPSFn>("SetTargetFPS");
        try {
            isMouseButtonPressed = loader->loadFunction<IsMouseButtonPressedFn>("IsMouseButtonPressed");
            getMouseX = loader->loadFunction<GetMouseXFn>("GetMouseX");
            getMouseY = loader->loadFunction<GetMouseYFn>("GetMouseY");
        } catch (...) {
        }
    } catch (const LibLoader::exception& e) {
        throw std::runtime_error(std::string("Failed to load Raylib: ") + e.what());
    }
}

arc::RaylibGraphics::~RaylibGraphics()
{
    if (initialized)
        close();
}

int arc::RaylibGraphics::convertColor(int color) const
{
    switch (color) {
        case 1: return RAYLIB_RED;
        case 2: return RAYLIB_GREEN;
        case 3: return RAYLIB_BLUE;
        case 4: return RAYLIB_BLACK;
        default: return RAYLIB_WHITE;
    }
}

void arc::RaylibGraphics::init()
{
    if (!initialized && initWindow) {
        initWindow(width, height, "Raylib Window");
        if (setTargetFPS)
            setTargetFPS(60);
        initialized = true;
    }
}

void arc::RaylibGraphics::close()
{
    if (initialized && closeWindow) {
        closeWindow();
        initialized = false;
    }
}

void arc::RaylibGraphics::clear()
{
    if (clearBackground)
        clearBackground(RAYLIB_BLACK);
}

void arc::RaylibGraphics::drawChar(int x, int y, char character, int color)
{
    std::string text(1, character);
    drawString(x, y, text, color);
}

void arc::RaylibGraphics::drawString(int x, int y, const std::string& text, int color)
{
    if (drawText && !text.empty()) {
        int raylibColor = convertColor(color);
        drawText(text.c_str(), x * 25, y * 25, 20, raylibColor);
    }
}

void arc::RaylibGraphics::refresh()
{
    if (beginDrawing && endDrawing) {
        beginDrawing();
        endDrawing();
    }
}

int arc::RaylibGraphics::_convertKeyCode(int raylibKey)
{
    switch (raylibKey) {
        case KEY_UP:    return static_cast<int>(StandardKeyCode::UP);
        case KEY_DOWN:  return static_cast<int>(StandardKeyCode::DOWN);
        case KEY_LEFT:  return static_cast<int>(StandardKeyCode::LEFT);
        case KEY_RIGHT: return static_cast<int>(StandardKeyCode::RIGHT);
        case KEY_ENTER: return static_cast<int>(StandardKeyCode::ENTER);
        case 'W': case 'w': return static_cast<int>(StandardKeyCode::KEY_W);
        case 'A': case 'a': return static_cast<int>(StandardKeyCode::KEY_A);
        case 'S': case 's': return static_cast<int>(StandardKeyCode::KEY_S);
        case 'D': case 'd': return static_cast<int>(StandardKeyCode::KEY_D);
        case 'Q': case 'q': return static_cast<int>(StandardKeyCode::KEY_Q);
        case 'M': case 'm': return static_cast<int>(StandardKeyCode::KEY_M);
        case 'N': case 'n': return static_cast<int>(StandardKeyCode::KEY_N);
        default: return raylibKey;
    }
}

arc::Event arc::RaylibGraphics::_handleEscapeAndWindowClose()
{
    arc::Event event;
    event.type = arc::EventType::None;
    event.key = 0;
    event.mouseX = 0;
    event.mouseY = 0;

    if (raylibIsKeyPressed && raylibIsKeyPressed(KEY_ESCAPE)) {
        event.type = arc::EventType::KeyPressed;
        event.key = static_cast<int>(StandardKeyCode::ESCAPE);
        return event;
    }
    if (windowShouldClose && windowShouldClose()) {
        event.type = arc::EventType::Quit;
        return event;
    }

    return event;
}

arc::Event arc::RaylibGraphics::_handleMouseInput()
{
    arc::Event event;
    event.type = arc::EventType::None;
    event.key = 0;
    event.mouseX = 0;
    event.mouseY = 0;

    if (isMouseButtonPressed && getMouseX && getMouseY) {
        if (isMouseButtonPressed(0)) {
            event.type = arc::EventType::MousePressed;
            event.mouseX = getMouseX() / 25;
            event.mouseY = getMouseY() / 25;
            event.key = 1;
            return event;
        } else if (isMouseButtonPressed(1)) {
            event.type = arc::EventType::MousePressed;
            event.mouseX = getMouseX() / 25;
            event.mouseY = getMouseY() / 25;
            event.key = 3;
            return event;
        }
    }

    return event;
}

arc::Event arc::RaylibGraphics::_handleKeyboardInput()
{
    arc::Event event;
    event.type = arc::EventType::None;
    event.key = 0;
    event.mouseX = 0;
    event.mouseY = 0;
    if (getKeyPressed) {
        int key = getKeyPressed();
        if (key != KEY_NULL) {
            event.type = arc::EventType::KeyPressed;
            event.key = _convertKeyCode(key);
        }
    }
    return event;
}

arc::Event arc::RaylibGraphics::getInput()
{
    arc::Event event = _handleEscapeAndWindowClose();
    if (event.type != arc::EventType::None)
        return event;
    event = _handleMouseInput();
    if (event.type != arc::EventType::None)
        return event;
    return _handleKeyboardInput();
}

int arc::RaylibGraphics::getWidth() const
{
    if (initialized && getScreenWidth)
        return getScreenWidth();
    return width;
}

int arc::RaylibGraphics::getHeight() const
{
    if (initialized && getScreenHeight)
        return getScreenHeight();
    return height;
}

bool arc::RaylibGraphics::isKeyPressed(int key) const
{
    if (raylibIsKeyPressed && initialized)
        return raylibIsKeyPressed(key);
    return false;
}

void arc::RaylibGraphics::setResolution(int w, int h)
{
    width = w;
    height = h;
    if (initialized) {
        close();
        init();
    }
}

bool arc::RaylibGraphics::isInitialized() const
{
    return initialized;
}

extern "C" {
    arc::IGraphics* createGraphics()
    {
        return new arc::RaylibGraphics();
    }

    void destroyGraphics(arc::IGraphics* graphics)
    {
        delete graphics;
    }
}
