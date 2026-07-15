/*
** EPITECH PROJECT, 2026
** piscine
** File description:
** InstructionExecutor
*/

#include "../include/VirtualMachine.hpp"
#include "../include/Operand.hpp"
#include <iostream>

// executeInstruction dispatcher
void VirtualMachine::executeInstruction(const Instruction& instr)
{
    switch (instr.getType())
    {
        case InstructionType::Push:
            executePush(instr); break;
        case InstructionType::Pop:
            executePop(); break;
        case InstructionType::Clear:
            executeClear(); break;
        case InstructionType::Swap:
            executeSwap(); break;
        case InstructionType::Display:
            executeDisplay(); break;
        case InstructionType::Assert:
            executeAssert(instr); break;
        case InstructionType::Add:
            executeAdd(); break;
        case InstructionType::Sub:
            executeSub(); break;
        case InstructionType::Mul:
            executeMul(); break;
        case InstructionType::Div:
            executeDiv(); break;
        case InstructionType::Mod:
            executeMod(); break;
        case InstructionType::Load:
            executeLoad(instr); break;
        case InstructionType::Store:
            executeStore(instr); break;
        case InstructionType::Exit:
            executeExit(); break;
        default:
            throw VMException("Unknown instruction");
    }
}

// Toutes les executeXXX sont exactement celles qu’on a codées précédemment
// Push
void VirtualMachine::executePush(const Instruction& instr)
{
    IOperand* op = OperandFactory::createOperand(
        instr.getOperandType(),
        instr.getOperandValue()
    );
    _pile.push(op);
}

// Pop
void VirtualMachine::executePop()
{
    IOperand* op = _pile.pop();
    delete op;
}

// Clear
void VirtualMachine::executeClear()
{
    _pile.cleanup();
}

// Swap
void VirtualMachine::executeSwap()
{
    if (_pile.size() < 2)
        throw VMException("Swap on stack with less than two values");

    IOperand* first = _pile.pop();
    IOperand* second = _pile.pop();

    _pile.push(first);
    _pile.push(second);
}

// Display
void VirtualMachine::executeDisplay()
{
    IOperand* top = _pile.top();
    std::cout << top->toString() << std::endl;
}

// Assert
void VirtualMachine::executeAssert(const Instruction& instr)
{
    if (_pile.empty())
        throw VMException("Assert on empty stack");

    IOperand* top = _pile.top();
    IOperand* expected = OperandFactory::createOperand(
        instr.getOperandType(),
        instr.getOperandValue()
    );

    if (top->getType() != expected->getType() ||
        top->toString() != expected->toString())
    {
        delete expected;
        throw VMException("Assert failed");
    }
    delete expected;
}

// Add
void VirtualMachine::executeAdd()
{
    if (_pile.size() < 2)
        throw VMException("Add on stack with less than two values");

    IOperand* v1 = _pile.pop();
    IOperand* v2 = _pile.pop();

    try {
        IOperand* result = *v2 + *v1;
        delete v1;
        delete v2;
        _pile.push(result);
    } catch (...) {
        delete v1;
        delete v2;
        throw;
    }
}

// Sub
void VirtualMachine::executeSub()
{
    if (_pile.size() < 2)
        throw VMException("Sub on stack with less than two values");

    IOperand* v1 = _pile.pop();
    IOperand* v2 = _pile.pop();

    try {
        IOperand* result = *v2 - *v1;
        delete v1;
        delete v2;
        _pile.push(result);
    } catch (...) {
        delete v1;
        delete v2;
        throw;
    }
}

// Mul
void VirtualMachine::executeMul()
{
    if (_pile.size() < 2)
        throw VMException("Mul on stack with less than two values");

    IOperand* v1 = _pile.pop();
    IOperand* v2 = _pile.pop();

    try {
        IOperand* result = *v2 * *v1;
        delete v1;
        delete v2;
        _pile.push(result);
    } catch (...) {
        delete v1;
        delete v2;
        throw;
    }
}

// Div
void VirtualMachine::executeDiv()
{
    if (_pile.size() < 2)
        throw VMException("Div on stack with less than two values");

    IOperand* v1 = _pile.pop();
    IOperand* v2 = _pile.pop();

    try {
        IOperand* result = *v2 / *v1;
        delete v1;
        delete v2;
        _pile.push(result);
    } catch (...) {
        delete v1;
        delete v2;
        throw;
    }
}

// Mod
void VirtualMachine::executeMod()
{
    if (_pile.size() < 2)
        throw VMException("Mod on stack with less than two values");

    IOperand* v1 = _pile.pop();
    IOperand* v2 = _pile.pop();

    try {
        IOperand* result = *v2 % *v1;
        delete v1;
        delete v2;
        _pile.push(result);
    } catch (...) {
        delete v1;
        delete v2;
        throw;
    }
}

// Load
void VirtualMachine::executeLoad(const Instruction& instr)
{
    IOperand* value = _register.load(instr.getRegisterIndex());
    IOperand* copy = OperandFactory::createOperand(value->getType(), value->toString());
    _pile.push(copy);
}

// Store
void VirtualMachine::executeStore(const Instruction& instr)
{
    IOperand* value = _pile.pop();
    _register.store(instr.getRegisterIndex(), value);
}

// Exit
void VirtualMachine::executeExit()
{
    _isrunning = false;
}
