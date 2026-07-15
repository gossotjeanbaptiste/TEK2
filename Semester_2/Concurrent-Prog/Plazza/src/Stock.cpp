/*
** EPITECH PROJECT, 2024
** Plazza
** File description:
** Stock class implementation
*/

#include "Stock.hpp"

using namespace Plazza;

Stock::Stock(uint32_t refillTime, uint32_t initialQuantity)
    : _refillTime(refillTime), _initialQuantity(initialQuantity)
{
    reset(initialQuantity);
}

void Stock::reset(uint32_t quantity)
{
    _ingredients.clear();
    _ingredients[IngredientType::DOUGH] = Ingredient(IngredientType::DOUGH, quantity);
    _ingredients[IngredientType::TOMATO] = Ingredient(IngredientType::TOMATO, quantity);
    _ingredients[IngredientType::GRUYERE] = Ingredient(IngredientType::GRUYERE, quantity);
    _ingredients[IngredientType::HAM] = Ingredient(IngredientType::HAM, quantity);
    _ingredients[IngredientType::MUSHROOMS] = Ingredient(IngredientType::MUSHROOMS, quantity);
    _ingredients[IngredientType::STEAK] = Ingredient(IngredientType::STEAK, quantity);
    _ingredients[IngredientType::EGGPLANT] = Ingredient(IngredientType::EGGPLANT, quantity);
    _ingredients[IngredientType::GOAT_CHEESE] = Ingredient(IngredientType::GOAT_CHEESE, quantity);
    _ingredients[IngredientType::CHIEF_LOVE] = Ingredient(IngredientType::CHIEF_LOVE, quantity);
}

bool Stock::hasIngredient(IngredientType type, uint32_t quantity) const
{
    auto it = _ingredients.find(type);
    if (it != _ingredients.end()) {
        return it->second.getQuantity() >= quantity;
    }
    return false;
}

bool Stock::hasAllIngredients(const std::map<IngredientType, uint32_t> &ingredients) const
{
    for (const auto &pair : ingredients) {
        if (!hasIngredient(pair.first, pair.second)) {
            return false;
        }
    }
    return true;
}

bool Stock::consumeIngredients(const std::map<IngredientType, uint32_t> &ingredients)
{
    // First check if we have everything
    if (!hasAllIngredients(ingredients))
        return false;

    // Then consume
    for (const auto &pair : ingredients) {
        auto it = _ingredients.find(pair.first);
        if (it != _ingredients.end())
            it->second.consume(pair.second);
    }
    return true;
}

void Stock::refillIngredients()
{
    for (auto &pair : _ingredients)
        if (pair.second.getQuantity() < _initialQuantity)
            pair.second.addQuantity(1);
}

uint32_t Stock::getQuantity(IngredientType type) const
{
    auto it = _ingredients.find(type);
    if (it != _ingredients.end())
        return it->second.getQuantity();
    return 0;
}
