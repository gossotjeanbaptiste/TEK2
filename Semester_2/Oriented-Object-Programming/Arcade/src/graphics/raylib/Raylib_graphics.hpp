#pragma once

#include "../../../include/IGraphics.hpp"
#include "../../../include/LibLoader.hpp"
#include <memory>
#include <string>

typedef void (*InitWindowFn)(int width, int height, const char* title);
typedef bool (*WindowShouldCloseFn)();
typedef void (*CloseWindowFn)();
typedef void (*BeginDrawingFn)();
typedef void (*EndDrawingFn)();
typedef void (*ClearBackgroundFn)(int color);
typedef void (*DrawTextFn)(const char* text, int posX, int posY, int fontSize, int color);
typedef bool (*IsKeyPressedFn)(int key);
typedef int (*GetKeyPressedFn)();
typedef int (*GetScreenWidthFn)();
typedef int (*GetScreenHeightFn)();
typedef void (*SetTargetFPSFn)(int fps);
typedef bool (*IsMouseButtonPressedFn)(int button);
typedef int (*GetMouseXFn)();
typedef int (*GetMouseYFn)();

namespace arc {

    enum RaylibKey {
        KEY_NULL = 0,
        KEY_SPACE = 32,
        KEY_ENTER = 257,
        KEY_ESCAPE = 256,
        KEY_UP = 265,
        KEY_DOWN = 264,
        KEY_LEFT = 263,
        KEY_RIGHT = 262,
        KEY_A = 65,
        KEY_B = 66,
        KEY_C = 67,
        KEY_D = 68,
        KEY_E = 69,
        KEY_F = 70,
        KEY_G = 71,
        KEY_H = 72,
        KEY_I = 73,
        KEY_J = 74,
        KEY_K = 75,
        KEY_L = 76,
        KEY_M = 77,
        KEY_N = 78,
        KEY_O = 79,
        KEY_P = 80,
        KEY_Q = 81,
        KEY_R = 82,
        KEY_S = 83,
        KEY_T = 84,
        KEY_U = 85,
        KEY_V = 86,
        KEY_W = 87,
        KEY_X = 88,
        KEY_Y = 89,
        KEY_Z = 90
    };

    class RaylibGraphics : public IGraphics {
        private:
            std::unique_ptr<arc::LibLoader> loader;

            InitWindowFn initWindow;
            WindowShouldCloseFn windowShouldClose;
            CloseWindowFn closeWindow;
            BeginDrawingFn beginDrawing;
            EndDrawingFn endDrawing;
            ClearBackgroundFn clearBackground;
            DrawTextFn drawText;
            GetScreenWidthFn getScreenWidth;
            GetScreenHeightFn getScreenHeight;
            IsKeyPressedFn raylibIsKeyPressed;
            GetKeyPressedFn getKeyPressed;
            SetTargetFPSFn setTargetFPS;
            IsMouseButtonPressedFn isMouseButtonPressed;
            GetMouseXFn getMouseX;
            GetMouseYFn getMouseY;

            int width;
            int height;
            bool initialized;

            static const int RAYLIB_BLACK;
            static const int RAYLIB_WHITE;
            static const int RAYLIB_RED;
            static const int RAYLIB_GREEN;
            static const int RAYLIB_BLUE;

            int convertColor(int color) const;
            
            int _convertKeyCode(int raylibKey);
            Event _handleEscapeAndWindowClose();
            Event _handleMouseInput();
            Event _handleKeyboardInput();

        public:
            RaylibGraphics(int width = 800, int height = 600);
            ~RaylibGraphics();

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

    std::unique_ptr<IGraphics> createRaylibGraphics(int width, int height);

}
