/*
** EPITECH PROJECT, 2024
** Plazza
** File description:
** Pizza class definition
*/

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <map>
#include <iostream>
#include "Ingredient.hpp"

namespace Plazza {

    /**
     * @enum PizzaType
     * @brief Types of pizzas available
     */
    enum class PizzaType : uint8_t
    {
        MARGARITA   = 1,
        REGINA      = 2,
        AMERICANA   = 4,
        FANTASIA    = 8
    };

    /**
     * @enum PizzaSize
     * @brief Sizes available for pizzas
     */
    enum class PizzaSize : uint8_t
    {
        S   = 1,
        M   = 2,
        L   = 4,
        XL  = 8,
        XXL = 16
    };

    /**
     * @struct PizzaRecipe
     * @brief Complete recipe definition for a pizza type
     * Combines all pizza metadata: type, name, cooking time, and ingredients
     */
    struct PizzaRecipe
    {
        PizzaType type;
        std::string name;
        uint32_t baseCookingTime;  // in seconds
        std::map<IngredientType, uint32_t> ingredients;
    };

    /**
     * @class Pizza
     * @brief Represents a pizza with type, size and cooking parameters
     */
    class Pizza
    {
        public:
            /**
             * @brief Construct a Pizza
             * @param type The type of pizza
             * @param size The size of the pizza
             * @param id Unique identifier for this pizza
             */
            Pizza(PizzaType type, PizzaSize size, uint32_t id = 0);

            /**
             * @brief Default constructor
             */
            Pizza();

            /**
             * @brief Destructor
             */
            ~Pizza() = default;

            /* Getters */
            PizzaType getType() const { return _type; }
            PizzaSize getSize() const { return _size; }
            uint32_t getId() const { return _id; }
            std::string getTypeName() const;
            std::string getSizeName() const;

            /**
             * @brief Get cooking time in milliseconds
             * @param multiplier Cooking time multiplier from command line
             * @return Cooking time in milliseconds
             */
            uint32_t getCookingTime(float multiplier) const;

            /**
             * @brief Get required ingredients for this pizza type
             * @return Map of ingredient type to quantity
             */
            std::map<IngredientType, uint32_t> getRequiredIngredients() const;

            /**
             * @brief Get base cooking time in seconds for this pizza type
             * @return Base cooking time in seconds
             */
            uint32_t getBaseCookingTime() const;

            /* Serialization operators for IPC */
            /**
             * @brief Pack pizza data into a byte array for transmission
             * @param data Output buffer to pack into
             * @return Number of bytes written
             */
            uint32_t pack(uint8_t *data) const;

            /**
             * @brief Unpack pizza data from a byte array
             * @param data Input buffer to unpack from
             * @return Number of bytes read
             */
            uint32_t unpack(const uint8_t *data);

            /**
             * @brief Get size needed for serialization
             * @return Number of bytes needed
             */
            static constexpr uint32_t getPackSize() { return 9; } // 1 type + 1 size + 4 id + 3 padding

        private:
            PizzaType _type;
            PizzaSize _size;
            uint32_t _id;
    };

    /* Stream operators for logging */
    std::ostream &operator<<(std::ostream &os, const Pizza &pizza);
    std::ostream &operator<<(std::ostream &os, PizzaType type);
    std::ostream &operator<<(std::ostream &os, PizzaSize size);

} // namespace Plazza

