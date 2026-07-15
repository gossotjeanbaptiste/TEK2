/*
** EPITECH PROJECT, 2026
** piscine
** File description:
** InstructionExecutor
*/

#pragma once

#include <string>
#include "../include/IOperand.hpp"

enum class InstructionType
{
    Push,
    Pop,
    Clear,
    Swap,
    Display,
    Assert,
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Load,
    Store,
    Exit
};

class Instruction
{
public:
    // Constructors
    Instruction(InstructionType type); // For instructions without operands
    Instruction(InstructionType type, OperandType operandType, const std::string& value); // Push / Assert
    Instruction(InstructionType type, std::size_t registerIndex); // Load / Store

    // Getters
    InstructionType getType() const;
    OperandType getOperandType() const;       // Only valid for Push / Assert
    const std::string& getOperandValue() const; // Only valid for Push / Assert
    std::size_t getRegisterIndex() const;     // Only valid for Load / Store

private:
    InstructionType _type;

    // Optional fields
    OperandType _operandType;      // Only for Push / Assert
    std::string _operandValue;     // Only for Push / Assert
    std::size_t _registerIndex;    // Only for Load / Store
};
