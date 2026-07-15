/*
** EPITECH PROJECT, 2024
** Plazza
** File description:
** PizzaBuilder class implementation
*/

#include "PizzaBuilder.hpp"
#include "PizzaRecipes.hpp"
#include <stdexcept>

using namespace Plazza;

PizzaBuilder::PizzaBuilder()
    : _type(PizzaType::MARGARITA), _size(PizzaSize::M), _id(0),
      _typeSet(false), _sizeSet(false)
{
}

PizzaBuilder &PizzaBuilder::setType(PizzaType type)
{
    _type = type;
    _typeSet = true;
    return *this;
}

PizzaBuilder &PizzaBuilder::setSize(PizzaSize size)
{
    _size = size;
    _sizeSet = true;
    return *this;
}

PizzaBuilder &PizzaBuilder::setId(uint32_t id)
{
    _id = id;
    return *this;
}

PizzaBuilder &PizzaBuilder::addIngredient(IngredientType ingredient, uint32_t quantity)
{
    _ingredients[ingredient] += quantity;
    return *this;
}

PizzaBuilder &PizzaBuilder::loadRecipe(PizzaType type)
{
    const auto &recipes = getPizzaRecipes();
    auto it = recipes.find(type);
    if (it == recipes.end())
        throw std::runtime_error("Unknown pizza type"); 
    
    _type = type;
    _typeSet = true;
    _ingredients = it->second.ingredients;
    
    return *this;
}

PizzaBuilder &PizzaBuilder::removeIngredient(IngredientType ingredient)
{
    _ingredients.erase(ingredient);
    return *this;
}

PizzaBuilder &PizzaBuilder::clearIngredients()
{
    _ingredients.clear();
    return *this;
}

PizzaBuilder &PizzaBuilder::reset()
{
    _type = PizzaType::MARGARITA;
    _size = PizzaSize::M;
    _id = 0;
    _typeSet = false;
    _sizeSet = false;
    _ingredients.clear();
    return *this;
}

Pizza PizzaBuilder::build() const
{
    if (!_typeSet)
        throw std::runtime_error("Pizza type not set");
    if (!_sizeSet)
        throw std::runtime_error("Pizza size not set");
    return Pizza(_type, _size, _id);
}
