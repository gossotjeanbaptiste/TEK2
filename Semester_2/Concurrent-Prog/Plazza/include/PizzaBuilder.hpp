/*
** EPITECH PROJECT, 2024
** Plazza
** File description:
** PizzaBuilder class definition
*/

#pragma once

#include "Pizza.hpp"
#include <map>

namespace Plazza {

/**
 * @class PizzaBuilder
 * @brief Builder pattern for constructing Pizza objects
 * Provides fluent API for building pizzas with custom ingredients
 */
    class PizzaBuilder
    {
        public:
            /**
             * @brief Constructor
             */
            PizzaBuilder();

            /**
             * @brief Set pizza type
             * @param type The pizza type
             * @return Reference to this builder for chaining
             */
            PizzaBuilder &setType(PizzaType type);

            /**
             * @brief Set pizza size
             * @param size The pizza size
             * @return Reference to this builder for chaining
             */
            PizzaBuilder &setSize(PizzaSize size);

            /**
             * @brief Set pizza ID
             * @param id Unique identifier
             * @return Reference to this builder for chaining
             */
            PizzaBuilder &setId(uint32_t id);

            /**
             * @brief Add an ingredient to the pizza
             * @param ingredient The ingredient type
             * @param quantity Amount to add
             * @return Reference to this builder for chaining
             */
            PizzaBuilder &addIngredient(IngredientType ingredient, uint32_t quantity);

            /**
             * @brief Load standard pizza recipe (replaces ingredients)
             * @param type The pizza type to load recipe from
             * @return Reference to this builder for chaining
             */
            PizzaBuilder &loadRecipe(PizzaType type);

            /**
             * @brief Remove an ingredient
             * @param ingredient The ingredient to remove
             * @return Reference to this builder for chaining
             */
            PizzaBuilder &removeIngredient(IngredientType ingredient);

            /**
             * @brief Clear all ingredients
             * @return Reference to this builder for chaining
             */
            PizzaBuilder &clearIngredients();

            /**
             * @brief Build the Pizza object
             * @return Constructed Pizza
             * @throws std::runtime_error if type or size not set
             */
            Pizza build() const;

            /**
             * @brief Reset builder to initial state
             * @return Reference to this builder for chaining
             */
            PizzaBuilder &reset();

        private:
            PizzaType _type;
            PizzaSize _size;
            uint32_t _id;
            std::map<IngredientType, uint32_t> _ingredients;
            bool _typeSet;
            bool _sizeSet;
    };

} // namespace Plazza
