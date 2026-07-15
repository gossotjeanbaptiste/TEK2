#include "SDL2_graphics.hpp"
#include "../../../include/KeyCodes.hpp"
#include <iostream>
#include <stdexcept>

namespace arc {

SDL2Graphics::SDL2Graphics(int width, int height) 
    : _window(nullptr), _renderer(nullptr), _font(nullptr), 
      _width(width), _height(height), _initialized(false), _lastFrameTime(0)
{
}

SDL2Graphics::~SDL2Graphics()
{
    if (_initialized) {
        close();
    }
}

SDL_Color SDL2Graphics::_convertColor(int color)
{
    SDL_Color c;
    switch (color) {
        case 1:
            c.r = 255; c.g = 0; c.b = 0; c.a = 255;
            break;
        case 2:
            c.r = 0; c.g = 255; c.b = 0; c.a = 255;
            break;
        case 3:
            c.r = 0; c.g = 0; c.b = 255; c.a = 255;
            break;
        case 4:
            c.r = 0; c.g = 0; c.b = 0; c.a = 255;
            break;
        default:
            c.r = 255; c.g = 255; c.b = 255; c.a = 255;
            break;
    }
    return c;
}

int SDL2Graphics::_convertKeyCode(int sdlKey)
{
    switch (sdlKey) {
        case SDLK_UP:
            return static_cast<int>(StandardKeyCode::UP);
        case SDLK_DOWN:
            return static_cast<int>(StandardKeyCode::DOWN);
        case SDLK_LEFT:
            return static_cast<int>(StandardKeyCode::LEFT);
        case SDLK_RIGHT:
            return static_cast<int>(StandardKeyCode::RIGHT);
        case SDLK_RETURN:
            return static_cast<int>(StandardKeyCode::ENTER);
        case SDLK_ESCAPE:
            return static_cast<int>(StandardKeyCode::ESCAPE);
        case 'w': case 'W':
            return static_cast<int>(StandardKeyCode::KEY_W);
        case 'a': case 'A':
            return static_cast<int>(StandardKeyCode::KEY_A);
        case 's': case 'S':
            return static_cast<int>(StandardKeyCode::KEY_S);
        case 'd': case 'D':
            return static_cast<int>(StandardKeyCode::KEY_D);
        case 'q': case 'Q':
            return static_cast<int>(StandardKeyCode::KEY_Q);
        case 'm': case 'M':
            return static_cast<int>(StandardKeyCode::KEY_M);
        case 'n': case 'N':
            return static_cast<int>(StandardKeyCode::KEY_N);
        case 'r': case 'R':
            return 'r';
        default:
            return sdlKey;
    }
}

void SDL2Graphics::init()
{
    if (_initialized) return;
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error("Failed to initialize SDL2");
    }
    
    if (TTF_Init() < 0) {
        SDL_Quit();
        throw std::runtime_error("Failed to initialize SDL2_ttf");
    }
    
    _window = SDL_CreateWindow("SDL2 Window", 
                                SDL_WINDOWPOS_CENTERED, 
                                SDL_WINDOWPOS_CENTERED, 
                                _width, _height, 
                                SDL_WINDOW_SHOWN);
    if (!_window) {
        TTF_Quit();
        SDL_Quit();
        throw std::runtime_error("Failed to create SDL2 window");
    }
    
    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_SOFTWARE);
    if (!_renderer) {
        SDL_DestroyWindow(_window);
        TTF_Quit();
        SDL_Quit();
        throw std::runtime_error("Failed to create SDL2 renderer");
    }
    
    _font = TTF_OpenFont("assets/font1.ttf", 24);
    if (!_font) {
        _font = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 24);
    }
    if (!_font) {
        _font = TTF_OpenFont("/usr/share/fonts/truetype/ubuntu/Ubuntu-M.ttf", 24);
    }
    if (!_font) {
        SDL_DestroyRenderer(_renderer);
        SDL_DestroyWindow(_window);
        TTF_Quit();
        SDL_Quit();
        throw std::runtime_error("Failed to load font");
    }
    _lastFrameTime = SDL_GetTicks();
    _initialized = true;
}

void SDL2Graphics::close()
{
    if (!_initialized) return;
    if (_font) {
        TTF_CloseFont(_font);
        _font = nullptr;
    }
    if (_renderer) {
        SDL_DestroyRenderer(_renderer);
        _renderer = nullptr;
    }
    if (_window) {
        SDL_DestroyWindow(_window);
        _window = nullptr;
    }
    TTF_Quit();
    SDL_Quit();
    _initialized = false;
}

void SDL2Graphics::clear()
{
    if (_renderer) {
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
        SDL_RenderClear(_renderer);
    }
}

void SDL2Graphics::drawChar(int x, int y, char character, int color)
{
    std::string text(1, character);
    drawString(x, y, text, color);
}

void SDL2Graphics::drawString(int x, int y, const std::string& text, int color)
{
    if (!_renderer || !_font) return;
    SDL_Color textColor = _convertColor(color);
    SDL_Surface* surface = TTF_RenderText_Solid(_font, text.c_str(), textColor);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }
    int pixelX = x * 15;
    int pixelY = y * 15;
    SDL_Rect destRect = {pixelX, pixelY, surface->w, surface->h};
    SDL_RenderCopy(_renderer, texture, NULL, &destRect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void SDL2Graphics::_limitFPS()
{
    Uint32 currentTime = SDL_GetTicks();
    Uint32 elapsedTime = currentTime - _lastFrameTime;
    
    if (elapsedTime < FRAME_DELAY) {
        SDL_Delay(FRAME_DELAY - elapsedTime);
    }
    
    _lastFrameTime = SDL_GetTicks();
}

void SDL2Graphics::refresh()
{
    _limitFPS();
    if (_renderer)
        SDL_RenderPresent(_renderer);
}

Event SDL2Graphics::getInput()
{
    Event event;
    event.type = EventType::None;
    event.key = 0;
    event.mouseX = 0;
    event.mouseY = 0;
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
        if (sdlEvent.type == SDL_QUIT) {
            event.type = EventType::Quit;
            return event;
        }
        if (sdlEvent.type == SDL_KEYDOWN) {
            event.type = EventType::KeyPressed;
            event.key = _convertKeyCode(sdlEvent.key.keysym.sym);
            return event;
        }
    }
    
    return event;
}

int SDL2Graphics::getWidth() const
{
    if (_initialized && _window) {
        int w, h;
        SDL_GetWindowSize(_window, &w, &h);
        return w / 15;
    }
    return _width / 15;
}

int SDL2Graphics::getHeight() const
{
    if (_initialized && _window) {
        int w, h;
        SDL_GetWindowSize(_window, &w, &h);
        return h / 15;
    }
    return _height / 15;
}

bool SDL2Graphics::isKeyPressed(int key) const
{
    const Uint8* state = SDL_GetKeyboardState(NULL);
    switch (key) {
        case static_cast<int>(StandardKeyCode::UP):
            return state[SDL_SCANCODE_UP];
        case static_cast<int>(StandardKeyCode::DOWN):
            return state[SDL_SCANCODE_DOWN];
        case static_cast<int>(StandardKeyCode::LEFT):
            return state[SDL_SCANCODE_LEFT];
        case static_cast<int>(StandardKeyCode::RIGHT):
            return state[SDL_SCANCODE_RIGHT];
        case static_cast<int>(StandardKeyCode::KEY_W):
            return state[SDL_SCANCODE_W];
        case static_cast<int>(StandardKeyCode::KEY_A):
            return state[SDL_SCANCODE_A];
        case static_cast<int>(StandardKeyCode::KEY_S):
            return state[SDL_SCANCODE_S];
        case static_cast<int>(StandardKeyCode::KEY_D):
            return state[SDL_SCANCODE_D];
        default:
            return false;
    }
}

void SDL2Graphics::setResolution(int w, int h)
{
    _width = w * 15;
    _height = h * 15;
    if (_initialized) {
        close();
        init();
    }
}

bool SDL2Graphics::isInitialized() const
{
    return _initialized;
}

}

extern "C" {
    arc::IGraphics* createGraphics()
    {
        return new arc::SDL2Graphics(800, 600);
    }

    void destroyGraphics(arc::IGraphics* graphics)
    {
        delete graphics;
    }
}
