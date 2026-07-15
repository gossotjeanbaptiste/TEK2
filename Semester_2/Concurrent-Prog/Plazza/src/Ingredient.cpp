/*
** EPITECH PROJECT, 2024
** Plazza
** File description:
** Ingredient class implementation
*/

#include "Ingredient.hpp"
#include <map>

using namespace Plazza;

/* Static ingredient name mapping */
static const std::map<IngredientType, std::string> INGREDIENT_NAMES = {
    {IngredientType::DOUGH, "dough"},
    {IngredientType::TOMATO, "tomato"},
    {IngredientType::GRUYERE, "gruyere"},
    {IngredientType::HAM, "ham"},
    {IngredientType::MUSHROOMS, "mushrooms"},
    {IngredientType::STEAK, "steak"},
    {IngredientType::EGGPLANT, "eggplant"},
    {IngredientType::GOAT_CHEESE, "goat_cheese"},
    {IngredientType::CHIEF_LOVE, "chief_love"}
};

Ingredient::Ingredient() : _type(IngredientType::DOUGH), _quantity(0)
{
}

Ingredient::Ingredient(IngredientType type, uint32_t quantity)
    : _type(type), _quantity(quantity)
{
}

std::string Ingredient::getTypeName() const
{
    auto it = INGREDIENT_NAMES.find(_type);
    if (it != INGREDIENT_NAMES.end())
        return it->second;
    return "unknown";
}

bool Ingredient::consume(uint32_t quantity)
{
    if (_quantity >= quantity) {
        _quantity -= quantity;
        return true;
    }
    return false;
}

/* Stream operators */
std::ostream &Plazza::operator<<(std::ostream &os, const Plazza::Ingredient &ingredient)
{
    os << ingredient.getTypeName() << ":" << ingredient.getQuantity();
    return os;
}

std::ostream &Plazza::operator<<(std::ostream &os, Plazza::IngredientType type)
{
    auto it = INGREDIENT_NAMES.find(type);
    if (it != INGREDIENT_NAMES.end())
        os << it->second;
    else
        os << "unknown";
    return os;
}
