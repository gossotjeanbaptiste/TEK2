/*
** EPITECH PROJECT, 2026
** piscine
** File description:
** VirtualMachine
*/

#include "../include/VirtualMachine.hpp"
#include "../include/Operand.hpp"
#include "../include/VMException.hpp"

VirtualMachine::VirtualMachine(): _isrunning(true)
{
}

VirtualMachine::~VirtualMachine()
{
    _pile.cleanup();
    _register.cleanup();
}

void VirtualMachine::run(const std::vector<Instruction>& instructions)
{
    _isrunning = true;
    for (std::size_t i = 0; i < instructions.size() && _isrunning; ++i)
        executeInstruction(instructions[i]);
}

