/*
** EPITECH PROJECT, 2026
** piscine
** File description:
** VirtualMachine
*/

#pragma once

#include "Stack.hpp"
#include "InstructionExecutor.hpp"
#include "RegisterBank.hpp"
#include <vector>

class VirtualMachine {
    public:
        VirtualMachine();
        ~VirtualMachine();

        void run(const std::vector<Instruction> &instruction);

    protected:
    private:
        Stack _pile;
        RegisterBank _register;
        bool _isrunning;
        //Instruction orchestration
        void executeInstruction(const Instruction &instruction);

        //one method per instruction
        void executePush(const Instruction &instruction);
        void executePop();
        void executeClear();
        void executeSwap();
        void executeDisplay();
        void executeAssert(const Instruction &instruction);

        void executeAdd();
        void executeDiv();
        void executeMod();
        void executeMul();
        void executeSub();

        void executeLoad(const Instruction &instruction);
        void executeStore(const Instruction &instruction);

        void executeExit();
};
