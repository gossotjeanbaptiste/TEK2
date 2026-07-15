/*
** EPITECH PROJECT, 2026
** piscine
** File description:
** RegisterBank
*/

#include "../include/RegisterBank.hpp"
#include "../include/Exception.hpp"

RegisterBank::RegisterBank()
{
    _registers.fill(nullptr);
}

RegisterBank::~RegisterBank()
{
    cleanup();
}

void RegisterBank::store(std::size_t index, IOperand* value)
{
    if (index >= _registers.size())
        throw RegisterException("Register index out of bounds");
    _registers[index] = value;
}

IOperand* RegisterBank::load(std::size_t index) const
{
    if (index >= _registers.size() || _registers[index] == nullptr)
        throw RegisterException("Load from empty register");
    return _registers[index];
}

void RegisterBank::clear()
{
    for (IOperand*& reg : _registers) {
        if (reg != nullptr)
            delete reg;
        reg = nullptr;
    }
}

void RegisterBank::cleanup()
{
    clear();
}
