/*
** EPITECH PROJECT, 2024
** Plazza
** File description:
** Stock class definition for ingredient management
*/

#pragma once

#include "Ingredient.hpp"
#include <map>
#include <cstdint>

namespace Plazza {

/**
 * @class Stock
 * @brief Manages ingredient inventory for a kitchen
 */
class Stock
{
    public:
        /**
         * @brief Constructor
         * @param refillTime Time in milliseconds for ingredients to regenerate
         * @param initialQuantity Initial quantity for each ingredient (default: 5)
         */
        Stock(uint32_t refillTime = 1000, uint32_t initialQuantity = 5);

        /**
         * @brief Destructor
         */
        ~Stock() = default;

        /**
         * @brief Check if we have enough of a specific ingredient
         * @param type Ingredient type
         * @param quantity Quantity needed
         * @return true if available, false otherwise
         */
        bool hasIngredient(IngredientType type, uint32_t quantity) const;

        /**
         * @brief Check if we have all required ingredients for a recipe
         * @param ingredients Map of ingredient type to quantity
         * @return true if all available, false otherwise
         */
        bool hasAllIngredients(const std::map<IngredientType, uint32_t> &ingredients) const;

        /**
         * @brief Consume ingredients (remove from stock)
         * @param ingredients Map of ingredient type to quantity
         * @return true if consumed successfully, false if not enough
         */
        bool consumeIngredients(const std::map<IngredientType, uint32_t> &ingredients);

        /**
         * @brief Refill ingredients by 1 unit
         * Should be called periodically based on refill time
         */
        void refillIngredients();

        /**
         * @brief Get all ingredients in stock
         * @return Map of all ingredients
         */
        const std::map<IngredientType, Ingredient> &getAllIngredients() const
        {
            return _ingredients;
        }

        /**
         * @brief Get quantity of a specific ingredient
         * @param type Ingredient type
         * @return Quantity available
         */
        uint32_t getQuantity(IngredientType type) const;

        /**
         * @brief Get refill time in milliseconds
         * @return Refill time
         */
        uint32_t getRefillTime() const { return _refillTime; }

        /**
         * @brief Reset stock to initial state
         * @param quantity Initial quantity for each ingredient
         */
        void reset(uint32_t quantity = 5);

    private:
        std::map<IngredientType, Ingredient> _ingredients;
        uint32_t _refillTime;      // Milliseconds between refill
        uint32_t _initialQuantity; // Initial/max quantity per ingredient
};

} // namespace Plazza
