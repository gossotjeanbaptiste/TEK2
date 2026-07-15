/*
** EPITECH PROJECT, 2024
** Arcade
** File description:
** IGame interface for game libraries
*/

#pragma once

#include <string>
#include "IEvents.hpp"

namespace arc {

    class IGraphics;

    class IGame {
        public:
            virtual ~IGame() = default;

            virtual void init() = 0;

            virtual void update(float deltaTime) = 0;

            virtual void render(IGraphics* graphics) = 0;

            virtual void handleInput(const Event& event) = 0;

            virtual bool isRunning() const = 0;

            virtual int getScore() const = 0;

            virtual std::string getPlayerName() const = 0;

            virtual void setPlayerName(const std::string& name) = 0;

            virtual void reset() = 0;

            virtual std::string getName() const = 0;

            virtual std::string getSelectedGame() const { return ""; }
            virtual std::string getSelectedGraphics() const { return ""; }
            virtual bool hasSelectedGame() const { return false; }
    };

    extern "C" {
        IGame* createGame();
        void destroyGame(IGame* game);
    }

}
