/*
** EPITECH PROJECT, 2026
** PLAZZA
** File description:
** Reception class - Interactive shell for pizza orders
*/

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include "CommandParser.hpp"
#include "Pizza.hpp"
#include "Kitchen.hpp"

namespace Plazza {

    class Logger
    {
    public:
        static void initialize();
        static void log(const std::string &message);

    private:
        static const char *_logFile;
    };

    class Reception
    {
    public:
        Reception(float cookingMultiplier, int cooksPerKitchen, int restockTimeMs);
        ~Reception();
        void run();

    private:
        float _cookingMultiplier;
        int _cooksPerKitchen;
        int _restockTimeMs;
        CommandParser _parser;
        
        std::vector<std::unique_ptr<Kitchen>> _kitchens;
        std::vector<std::thread> _kitchenThreads;
        std::mutex _kitchensMutex;
        int _nextKitchenId;

        void displayPrompt() const;
        bool handleCommand(const std::string &command);
        void processOrder(const std::string &orderString);
        void displayStatus() const;
        
        int createKitchen();
        void dispatchPizzas(const std::vector<Pizza> &pizzas);
        int findBestKitchen();
        void shutdownAllKitchens();
    };

} // namespace Plazza

