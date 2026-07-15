/*
** EPITECH PROJECT, 2024
** Plazza
** File description:
** Cook class implementation
*/

#include "Cook.hpp"
#include <stdexcept>
#include <chrono>
#include <thread>

using namespace Plazza;

Cook::Cook(float cookingMultiplier)
    : _cookingMultiplier(cookingMultiplier)
{
    if (cookingMultiplier <= 0.0f)
        throw std::invalid_argument("Cooking multiplier must be > 0");
}

Pizza Cook::cook(const Pizza &pizza, Stock &stock)
{
    // Get required ingredients for this pizza
    auto requiredIngredients = pizza.getRequiredIngredients();
    // Check if all ingredients are available
    if (!stock.hasAllIngredients(requiredIngredients))
        throw std::runtime_error("Not enough ingredients to cook pizza");
    // Consume ingredients from stock
    if (!stock.consumeIngredients(requiredIngredients))
        throw std::runtime_error("Failed to consume ingredients");
    // Calculate cooking time (already in milliseconds from pizza recipe)
    uint32_t cookingTimeMs = pizza.getCookingTime(_cookingMultiplier);
    
    // Simulate cooking by sleeping
    std::this_thread::sleep_for(std::chrono::milliseconds(cookingTimeMs));
    // Return the cooked pizza
    return pizza;
}

void Cook::setCookingMultiplier(float multiplier)
{
    if (multiplier <= 0.0f)
        throw std::invalid_argument("Cooking multiplier must be > 0");
    _cookingMultiplier = multiplier;
}
