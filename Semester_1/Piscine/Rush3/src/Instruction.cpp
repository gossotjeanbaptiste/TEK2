/*
** EPITECH PROJECT, 2026
** piscine
** File description:
** Instruction Implementation
*/

#include "../include/InstructionExecutor.hpp"

// Constructor for simple instructions (no operands)
Instruction::Instruction(InstructionType type)
    : _type(type), _operandType(OperandType::Int8), _operandValue(""), _registerIndex(0)
{
}

// Constructor for Push / Assert instructions
Instruction::Instruction(InstructionType type, OperandType operandType, const std::string& value)
    : _type(type), _operandType(operandType), _operandValue(value), _registerIndex(0)
{
}

// Constructor for Load / Store instructions
Instruction::Instruction(InstructionType type, std::size_t registerIndex)
    : _type(type), _operandType(OperandType::Int8), _operandValue(""), _registerIndex(registerIndex)
{
}

// Getters
InstructionType Instruction::getType() const
{
    return _type;
}

OperandType Instruction::getOperandType() const
{
    return _operandType;
}

const std::string& Instruction::getOperandValue() const
{
    return _operandValue;
}

std::size_t Instruction::getRegisterIndex() const
{
    return _registerIndex;
}
