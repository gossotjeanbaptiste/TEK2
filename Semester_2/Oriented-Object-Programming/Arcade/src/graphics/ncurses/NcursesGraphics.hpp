/*
** EPITECH PROJECT, 2024
** Arcade
** File description:
** NcursesGraphics implementation
*/

#pragma once

#include "../../../include/IGraphics.hpp"
#include <ncurses.h>

namespace arc {

    class NcursesGraphics : public IGraphics {
        private:
            int _width;
            int _height;
            bool _initialized;
            
            void _processMouseEvent(Event& event);
            void _processKeyboardEvent(int ch, Event& event);

        public:
            NcursesGraphics();
            virtual ~NcursesGraphics() = default;

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
    };

}
