/*
** EPITECH PROJECT, 2024
** Plazza
** File description:
** Pizza recipes database implementation
*/

#include "PizzaRecipes.hpp"
#include <map>

using namespace Plazza;

/**
 * @brief Static pizza recipes database
 * Single source of truth for all standard pizza recipes
 */
static const std::map<PizzaType, PizzaRecipe> PIZZA_RECIPES_DB = {
    {
        PizzaType::MARGARITA,
        {
            PizzaType::MARGARITA,
            "margarita",
            1,  // 1 second
            {
                {IngredientType::DOUGH, 1},
                {IngredientType::TOMATO, 1},
                {IngredientType::GRUYERE, 1}
            }
        }
    },
    {
        PizzaType::REGINA,
        {
            PizzaType::REGINA,
            "regina",
            2,  // 2 seconds
            {
                {IngredientType::DOUGH, 1},
                {IngredientType::TOMATO, 1},
                {IngredientType::GRUYERE, 1},
                {IngredientType::HAM, 1},
                {IngredientType::MUSHROOMS, 1}
            }
        }
    },
    {
        PizzaType::AMERICANA,
        {
            PizzaType::AMERICANA,
            "americana",
            2,  // 2 seconds
            {
                {IngredientType::DOUGH, 1},
                {IngredientType::TOMATO, 1},
                {IngredientType::GRUYERE, 1},
                {IngredientType::STEAK, 1}
            }
        }
    },
    {
        PizzaType::FANTASIA,
        {
            PizzaType::FANTASIA,
            "fantasia",
            4,  // 4 seconds
            {
                {IngredientType::DOUGH, 1},
                {IngredientType::TOMATO, 1},
                {IngredientType::EGGPLANT, 1},
                {IngredientType::GOAT_CHEESE, 1},
                {IngredientType::CHIEF_LOVE, 1}
            }
        }
    }
};

const std::map<PizzaType, PizzaRecipe> &Plazza::getPizzaRecipes()
{
    return PIZZA_RECIPES_DB;
}
