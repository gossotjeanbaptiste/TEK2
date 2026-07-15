/*
** EPITECH PROJECT, 2026
** PLAZZA
** File description:
** CommandParser class implementation
*/

#include "CommandParser.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>

using namespace Plazza;

std::string CommandParser::trim(const std::string &str)
{
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos)
        return "";
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> CommandParser::split(const std::string &str, char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, delimiter)) {
        item = trim(item);
        if (!item.empty())
            tokens.push_back(item);
    }
    return tokens;
}

bool CommandParser::isValidType(const std::string &type)
{
    std::string lower = type;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    return lower == "regina" || lower == "margarita" || 
           lower == "americana" || lower == "fantasia";
}

bool CommandParser::isValidSize(const std::string &size)
{
    std::string upper = size;
    std::transform(upper.begin(), upper.end(), upper.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    
    return upper == "S" || upper == "M" || upper == "L" || 
           upper == "XL" || upper == "XXL";
}

int CommandParser::parseQuantity(const std::string &quantityStr)
{
    // Format: x[1-9][0-9]*
    if (quantityStr.empty() || quantityStr[0] != 'x')
        throw std::invalid_argument("Invalid quantity format: " + quantityStr);
    
    if (quantityStr.size() < 2)
        throw std::invalid_argument("Quantity must have at least one digit");
    std::string numStr = quantityStr.substr(1);
    
    // Validate all characters are digits
    if (!std::all_of(numStr.begin(), numStr.end(), ::isdigit))
        throw std::invalid_argument("Quantity digits invalid: " + numStr);
    
    // First digit must be 1-9
    if (numStr[0] == '0')
        throw std::invalid_argument("Quantity cannot start with 0");
    
    try {
        int quantity = std::stoi(numStr);
        if (quantity <= 0)
            throw std::invalid_argument("Quantity must be positive");
        return quantity;
    } catch (const std::out_of_range &e) {
        throw std::invalid_argument("Quantity too large");
    }
}

std::vector<Pizza> CommandParser::parsePizzaItem(const std::string &item)
{
    std::string trimmed = trim(item);
    if (trimmed.empty())
        throw std::invalid_argument("Empty pizza item");
    // Split by spaces to get: TYPE SIZE QUANTITY
    std::vector<std::string> parts = split(trimmed, ' ');
    if (parts.size() != 3)
        throw std::invalid_argument
        ("Invalid pizza item format: " + item + " (expected: TYPE SIZE NUMBER)");
    std::string type = parts[0];
    std::string size = parts[1];
    std::string quantity = parts[2];
    // Validate type
    if (!isValidType(type))
        throw std::invalid_argument("Invalid pizza type: " + type);
    // Validate size
    if (!isValidSize(size))
        throw std::invalid_argument("Invalid pizza size: " + size);
    // Parse quantity
    int count = parseQuantity(quantity);
    // Create pizzas
    std::vector<Pizza> pizzas;
    try {
        PizzaType pizzaType = stringToPizzaType(type);
        PizzaSize pizzaSize = stringToPizzaSize(size);
        for (int i = 0; i < count; ++i) {
            pizzas.push_back(Pizza(pizzaType, pizzaSize, i));
        }
    } catch (const std::invalid_argument &e) {
        throw std::invalid_argument("Failed to create pizza: " + std::string(e.what()));
    }
    return pizzas;
}

bool CommandParser::isValidOrder(const std::string &command) const
{
    try {
        const_cast<CommandParser*>(this)->parseOrder(command);
        return true;
    } catch (const std::exception &) {
        return false;
    }
}

std::vector<Pizza> CommandParser::parseOrder(const std::string &command)
{
    std::string trimmed = trim(command);
    
    if (trimmed.empty())
        throw std::invalid_argument("Empty order");
    // Split by semicolon to get individual pizza items
    std::vector<std::string> items = split(trimmed, ';');
    if (items.empty())
        throw std::invalid_argument("No pizza items in order");
    std::vector<Pizza> allPizzas;
    // Parse each item
    for (const auto &item : items) {
        try {
            std::vector<Pizza> itemPizzas = parsePizzaItem(item);
            allPizzas.insert(allPizzas.end(), itemPizzas.begin(), itemPizzas.end());
        }
        catch (const std::invalid_argument &e) {
            throw std::invalid_argument("Error parsing order: " + std::string(e.what()));
        }
    }
    if (allPizzas.empty())
        throw std::invalid_argument("No valid pizzas in order");
    return allPizzas;
}

PizzaType CommandParser::stringToPizzaType(const std::string &type)
{
    std::string lower = type;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (lower == "margarita")
        return PizzaType::MARGARITA;
    if (lower == "regina")
        return PizzaType::REGINA;
    if (lower == "americana")
        return PizzaType::AMERICANA;
    if (lower == "fantasia")
        return PizzaType::FANTASIA;
    
    throw std::invalid_argument("Unknown pizza type: " + type);
}

PizzaSize CommandParser::stringToPizzaSize(const std::string &size)
{
    std::string upper = size;
    std::transform(upper.begin(), upper.end(), upper.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    if (upper == "S")
        return PizzaSize::S;
    if (upper == "M")
        return PizzaSize::M;
    if (upper == "L")
        return PizzaSize::L;
    if (upper == "XL")
        return PizzaSize::XL;
    if (upper == "XXL")
        return PizzaSize::XXL;
    throw std::invalid_argument("Unknown pizza size: " + size);
}

bool CommandParser::isStatusCommand(const std::string &command)
{
    std::string trimmed = trim(command);
    return trimmed == "status";
}

bool CommandParser::isExitCommand(const std::string &command)
{
    std::string trimmed = trim(command);
    std::string lower = trimmed;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return lower == "quit";
}
