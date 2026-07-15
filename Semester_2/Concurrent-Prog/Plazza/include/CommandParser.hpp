/*
** EPITECH PROJECT, 2026
** PLAZZA
** File description:
** CommandParser class
*/

#pragma once

#include <string>
#include <vector>
#include "Pizza.hpp"

using namespace Plazza;

class CommandParser
{
public:
    CommandParser() = default;
    ~CommandParser() = default;

    std::vector<Pizza> parseOrder(const std::string &command);
    bool isValidOrder(const std::string &command) const;
    static bool isStatusCommand(const std::string &command);
    static bool isExitCommand(const std::string &command);

private:

    std::vector<Pizza> parsePizzaItem(const std::string &item);

    static std::string trim(const std::string &str);
    static bool isValidType(const std::string &type);
    static bool isValidSize(const std::string &size);
    static int parseQuantity(const std::string &quantityStr);
    static std::vector<std::string> split(const std::string &str, char delimiter);
    static PizzaType stringToPizzaType(const std::string &type);
    static PizzaSize stringToPizzaSize(const std::string &size);
};
