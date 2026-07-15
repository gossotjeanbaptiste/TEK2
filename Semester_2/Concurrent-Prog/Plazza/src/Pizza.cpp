/*
** EPITECH PROJECT, 2024
** Plazza
** File description:
** Pizza class implementation
*/

#include "Pizza.hpp"
#include "PizzaRecipes.hpp"
#include <cstring>
#include <map>

using namespace Plazza;

/**
 * @brief Pizza size name mapping
 */
static const std::map<PizzaSize, std::string> PIZZA_SIZE_NAMES = {
    {PizzaSize::S, "S"},
    {PizzaSize::M, "M"},
    {PizzaSize::L, "L"},
    {PizzaSize::XL, "XL"},
    {PizzaSize::XXL, "XXL"}
};

Pizza::Pizza() : _type(PizzaType::MARGARITA), _size(PizzaSize::M), _id(0)
{
}

Pizza::Pizza(PizzaType type, PizzaSize size, uint32_t id)
    : _type(type), _size(size), _id(id)
{
}

std::string Pizza::getTypeName() const
{
    const auto &recipes = getPizzaRecipes();
    auto it = recipes.find(_type);
    if (it != recipes.end())
        return it->second.name;
    return "unknown";
}

std::string Pizza::getSizeName() const
{
    auto it = PIZZA_SIZE_NAMES.find(_size);
    if (it != PIZZA_SIZE_NAMES.end())
        return it->second;
    return "unknown";
}

uint32_t Pizza::getBaseCookingTime() const
{
    const auto &recipes = getPizzaRecipes();
    auto it = recipes.find(_type);
    if (it != recipes.end())
        return it->second.baseCookingTime;
    return 0;
}

uint32_t Pizza::getCookingTime(float multiplier) const
{
    uint32_t baseTime = getBaseCookingTime();
    return static_cast<uint32_t>(baseTime * 1000 * multiplier); // Convert to milliseconds
}

std::map<IngredientType, uint32_t> Pizza::getRequiredIngredients() const
{
    const auto &recipes = getPizzaRecipes();
    auto it = recipes.find(_type);
    if (it != recipes.end())
        return it->second.ingredients;
    return {};
}

uint32_t Pizza::pack(uint8_t *data) const
{
    if (!data)
        return 0;
    uint32_t offset = 0;
    // Pack type (1 byte)
    data[offset++] = static_cast<uint8_t>(_type);
    // Pack size (1 byte)
    data[offset++] = static_cast<uint8_t>(_size);
    // Pack id (4 bytes, little endian)
    std::memcpy(data + offset, &_id, sizeof(_id));
    offset += sizeof(_id);
    // Padding to maintain 8-byte alignment (3 bytes)
    data[offset++] = 0;
    data[offset++] = 0;
    data[offset++] = 0;

    return offset;
}

uint32_t Pizza::unpack(const uint8_t *data)
{
    if (!data)
        return 0;
    uint32_t offset = 0;
    // Unpack type (1 byte)
    _type = static_cast<PizzaType>(data[offset++]);
    // Unpack size (1 byte)
    _size = static_cast<PizzaSize>(data[offset++]);
    // Unpack id (4 bytes, little endian)
    std::memcpy(&_id, data + offset, sizeof(_id));
    offset += sizeof(_id);
    // Skip padding (3 bytes)
    offset += 3;

    return offset;
}

/* Stream operators */
std::ostream &Plazza::operator<<(std::ostream &os, const Plazza::Pizza &pizza)
{
    os << pizza.getTypeName() << " " << pizza.getSizeName() << " #" << pizza.getId();
    return os;
}

std::ostream &Plazza::operator<<(std::ostream &os, Plazza::PizzaType type)
{
    const auto &recipes = Plazza::getPizzaRecipes();
    auto it = recipes.find(type);
    if (it != recipes.end())
        os << it->second.name;
    else
        os << "unknown";
    return os;
}

std::ostream &Plazza::operator<<(std::ostream &os, Plazza::PizzaSize size)
{
    static const std::map<Plazza::PizzaSize, std::string> PIZZA_SIZE_NAMES = {
        {Plazza::PizzaSize::S, "S"},
        {Plazza::PizzaSize::M, "M"},
        {Plazza::PizzaSize::L, "L"},
        {Plazza::PizzaSize::XL, "XL"},
        {Plazza::PizzaSize::XXL, "XXL"}
    };
    auto it = PIZZA_SIZE_NAMES.find(size);
    if (it != PIZZA_SIZE_NAMES.end())
        os << it->second;
    else
        os << "unknown";
    return os;
}
