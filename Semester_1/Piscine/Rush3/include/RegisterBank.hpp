/*
** EPITECH PROJECT, 2026
** piscine
** File description:
** RegisterBank
*/

#pragma once

#include <array>
#include "../include/IOperand.hpp"
#include "VMException.hpp"

class RegisterBank
{
public:
    RegisterBank();
    ~RegisterBank();

    void store(std::size_t index, IOperand* value);
    IOperand* load(std::size_t index) const;

    void clear();
    void cleanup(); // Supprime vraiment les operands

private:
    std::array<IOperand*, 16> _registers;
};
