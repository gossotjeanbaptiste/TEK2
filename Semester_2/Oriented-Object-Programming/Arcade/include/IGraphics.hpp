/*
** EPITECH PROJECT, 2024
** Arcade
** File description:
** IGraphics interface for graphical libraries
*/

#pragma once

#include <string>
#include "IEvents.hpp"

namespace arc {

    class IGraphics {
        public:
            virtual ~IGraphics() = default;

            virtual void init() = 0;

            virtual void close() = 0;

            virtual void clear() = 0;

            virtual void drawChar(int x, int y, char character, int color = 0) = 0;

            virtual void drawString(int x, int y, const std::string& text, int color = 0) = 0;

            virtual void refresh() = 0;

            virtual Event getInput() = 0;

            virtual int getWidth() const = 0;

            virtual int getHeight() const = 0;

            virtual bool isKeyPressed(int key) const = 0;
    };

    extern "C" {
        IGraphics* createGraphics();
        void destroyGraphics(IGraphics* graphics);
    }

}
