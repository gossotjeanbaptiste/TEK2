/*
** EPITECH PROJECT, 2024
** Plazza
** File description:
** Ingredient class definition
*/

#pragma once

#include <string>
#include <cstdint>
#include <iostream>

namespace Plazza {

/**
 * @enum IngredientType
 * @brief Types of ingredients used in pizzas
 */
enum class IngredientType : uint8_t
{
    DOUGH       = 0,
    TOMATO      = 1,
    GRUYERE     = 2,
    HAM         = 3,
    MUSHROOMS   = 4,
    STEAK       = 5,
    EGGPLANT    = 6,
    GOAT_CHEESE = 7,
    CHIEF_LOVE  = 8
};

/**
 * @class Ingredient
 * @brief Represents an ingredient with quantity tracking
 */
class Ingredient
{
    public:
        /**
         * @brief Constructor
         * @param type The ingredient type
         * @param quantity Initial quantity
         */
        Ingredient(IngredientType type, uint32_t quantity = 0);

        /**
         * @brief Default constructor
         */
        Ingredient();

        /**
         * @brief Destructor
         */
        ~Ingredient() = default;

        /* Getters */
        IngredientType getType() const { return _type; }
        uint32_t getQuantity() const { return _quantity; }
        std::string getTypeName() const;

        /* Setters */
        void setQuantity(uint32_t quantity) { _quantity = quantity; }
        void addQuantity(uint32_t quantity) { _quantity += quantity; }
        
        /**
         * @brief Try to consume quantity of ingredient
         * @param quantity Amount to consume
         * @return true if consumed successfully, false if not enough
         */
        bool consume(uint32_t quantity);

    private:
        IngredientType _type;
        uint32_t _quantity;
};

/* Stream operators */
std::ostream &operator<<(std::ostream &os, const Ingredient &ingredient);
std::ostream &operator<<(std::ostream &os, IngredientType type);

} // namespace Plazza
