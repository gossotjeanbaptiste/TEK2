#pragma once

#include "../../../include/IGraphics.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

namespace arc {

    class SDL2Graphics : public IGraphics {
        private:
            SDL_Window* _window;
            SDL_Renderer* _renderer;
            TTF_Font* _font;
            int _width;
            int _height;
            bool _initialized;
            Uint32 _lastFrameTime;
            static const int MAX_FPS = 60;
            static const int FRAME_DELAY = 1000 / MAX_FPS; // 16ms for 60 FPS

            SDL_Color _convertColor(int color);
            int _convertKeyCode(int sdlKey);
            void _limitFPS();

        public:
            SDL2Graphics(int width = 800, int height = 600);
            ~SDL2Graphics();

            void init() override;
            void close() override;
            void clear() override;
            void drawChar(int x, int y, char character, int color = 0) override;
            void drawString(int x, int y, const std::string& text, int color = 0) override;
            void refresh() override;
            Event getInput() override;
            int getWidth() const override;
            int getHeight() const override;
            bool isKeyPressed(int key) const override;

            void setResolution(int w, int h);
            bool isInitialized() const;
    };

}

extern "C" {
    arc::IGraphics* createGraphics();
    void destroyGraphics(arc::IGraphics* graphics);
}
