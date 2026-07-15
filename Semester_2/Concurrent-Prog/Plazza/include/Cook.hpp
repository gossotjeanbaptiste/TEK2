/*
** EPITECH PROJECT, 2024
** Plazza
** File description:
** Cook class definition - represents a kitchen worker thread
*/
#pragma once

#include "Pizza.hpp"
#include "Stock.hpp"

namespace Plazza {
    /**
     * @class Cook
     * @brief Represents a cook (worker thread) in a kitchen
     * A cook prepares one pizza at a time, checking ingredients from stock
     * and simulating cooking time based on pizza recipe and multiplier
     */
    class Cook {
        public:
            /**
             * @brief Constructor - creates a cook with a cooking time multiplier
             * @param cookingMultiplier Multiplier for cooking time (from CLI args)
             *                           Default 1.0f means normal speed
             * @throws std::invalid_argument if multiplier is <= 0
             */
            explicit Cook(float cookingMultiplier = 1.0f);
            /**
             * @brief Cook a pizza by checking stock and simulating cooking time
             * @param pizza The pizza to cook
             * @param stock Kitchen stock to consume ingredients from
             * @return The cooked pizza (same as input, represents completion)
             * @throws std::runtime_error if ingredients not available or cooking fails
             */
            Pizza cook(const Pizza &pizza, Stock &stock);
            /**
             * @brief Get the cooking multiplier
             * @return Current cooking time multiplier
             */
            float getCookingMultiplier() const { return _cookingMultiplier; }
            /**
             * @brief Set the cooking multiplier
             * @param multiplier New cooking time multiplier
             * @throws std::invalid_argument if multiplier is <= 0
             */
            void setCookingMultiplier(float multiplier);
        private:
            float _cookingMultiplier;
    };

}
