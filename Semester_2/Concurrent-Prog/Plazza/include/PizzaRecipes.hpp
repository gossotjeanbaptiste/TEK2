/*
** EPITECH PROJECT, 2024
** Plazza
** File description:
** Pizza recipes database definition
*/

#pragma once

#include "Pizza.hpp"
#include <map>

namespace Plazza {

/**
 * @brief Get the pizza recipes database
 * @return Map of all standard pizza recipes
 */
    const std::map<PizzaType, PizzaRecipe> &getPizzaRecipes();

} // namespace Plazza

