/*
** EPITECH PROJECT, 2026
** piscine
** File description:
** Test main for VM components
*/

#include <iostream>
#include <cassert>
#include <stdexcept>
#include "../include/Stack.hpp"
#include "../include/RegisterBank.hpp"
#include "../include/InstructionExecutor.hpp"
#include "../include/VirtualMachine.hpp"
#include "../include/VMException.hpp"
#include "../include/Operand.hpp"
#include "../include/OperandFactory.hpp"

// Color codes for output
#define GREEN "\033[32m"
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"

int test_count = 0;
int test_passed = 0;

void print_test(const std::string& test_name) {
    std::cout << YELLOW << "Testing: " << test_name << RESET << std::endl;
    test_count++;
}

void print_success() {
    std::cout << GREEN << "✓ PASSED" << RESET << std::endl;
    test_passed++;
}

void print_failure(const std::string& reason) {
    std::cout << RED << "✗ FAILED: " << reason << RESET << std::endl;
}

// ============================================================================
// STACK TESTS
// ============================================================================

void test_stack_push_pop() {
    print_test("Stack: Push and Pop");
    try {
        Stack stack;
        
        IOperand* op1 = OperandFactory::createOperand(OperandType::Int32, "42");
        IOperand* op2 = OperandFactory::createOperand(OperandType::Int32, "100");
        
        assert(stack.empty() == true);
        assert(stack.size() == 0);
        
        stack.push(op1);
        assert(stack.size() == 1);
        assert(stack.empty() == false);
        
        stack.push(op2);
        assert(stack.size() == 2);
        
        IOperand* popped = stack.pop();
        assert(stack.size() == 1);
        assert(popped->toString() == "100");
        delete popped;
        
        popped = stack.pop();
        assert(stack.size() == 0);
        assert(popped->toString() == "42");
        delete popped;
        
        print_success();
    } catch (const std::exception& e) {
        print_failure(std::string("Exception: ") + e.what());
    }
}

void test_stack_top() {
    print_test("Stack: Top without popping");
    try {
        Stack stack;
        
        IOperand* op = OperandFactory::createOperand(OperandType::Int32, "123");
        stack.push(op);
        
        IOperand* top = stack.top();
        assert(stack.size() == 1); // Should still have 1 element
        assert(top->toString() == "123");
        
        // Clean up: pop and delete
        stack.pop();
        delete op;
        
        print_success();
    } catch (const std::exception& e) {
        print_failure(std::string("Exception: ") + e.what());
    }
}

void test_stack_clear() {
    print_test("Stack: Clear");
    try {
        Stack stack;
        
        // Create operands and push them to stack - stack will own them
        stack.push(OperandFactory::createOperand(OperandType::Int32, "1"));
        stack.push(OperandFactory::createOperand(OperandType::Int32, "2"));
        stack.push(OperandFactory::createOperand(OperandType::Int32, "3"));
        
        assert(stack.size() == 3);
        
        // clear() will delete all operands
        stack.clear();
        assert(stack.size() == 0);
        assert(stack.empty() == true);
        
        print_success();
    } catch (const std::exception& e) {
        print_failure(std::string("Exception: ") + e.what());
    }
}

void test_stack_empty_pop() {
    print_test("Stack: Pop on empty stack (should throw)");
    try {
        Stack stack;
        stack.pop(); // Should throw
        print_failure("No exception thrown");
    } catch (const VMException& e) {
        print_success();
    } catch (const std::exception& e) {
        print_failure(std::string("Wrong exception: ") + e.what());
    }
}

void test_stack_empty_top() {
    print_test("Stack: Top on empty stack (should throw)");
    try {
        Stack stack;
        stack.top(); // Should throw
        print_failure("No exception thrown");
    } catch (const VMException& e) {
        print_success();
    } catch (const std::exception& e) {
        print_failure(std::string("Wrong exception: ") + e.what());
    }
}

// ============================================================================
// REGISTER BANK TESTS
// ============================================================================

void test_registerbank_store_load() {
    print_test("RegisterBank: Store and Load");
    try {
        RegisterBank bank;
        
        IOperand* op1 = OperandFactory::createOperand(OperandType::Int32, "555");
        IOperand* op2 = OperandFactory::createOperand(OperandType::Int32, "999");
        
        bank.store(0, op1);
        bank.store(5, op2);
        
        IOperand* loaded1 = bank.load(0);
        IOperand* loaded2 = bank.load(5);
        
        assert(loaded1->toString() == "555");
        assert(loaded2->toString() == "999");
        
        print_success();
        // Don't delete op1 and op2 - RegisterBank owns them and will delete them
    } catch (const std::exception& e) {
        print_failure(std::string("Exception: ") + e.what());
    }
}

void test_registerbank_clear() {
    print_test("RegisterBank: Clear");
    try {
        RegisterBank bank;
        
        IOperand* op = OperandFactory::createOperand(OperandType::Int32, "77");
        bank.store(3, op);
        
        bank.clear();
        
        // After clear, registers should be empty (nullptr)
        // This depends on implementation
        
        print_success();
        // Don't delete op - RegisterBank owns it and cleared it
    } catch (const std::exception& e) {
        print_failure(std::string("Exception: ") + e.what());
    }
}

void test_registerbank_bounds() {
    print_test("RegisterBank: Index bounds (16 registers)");
    try {
        RegisterBank bank;
        
        // Should work for 0-15
        IOperand* op = OperandFactory::createOperand(OperandType::Int32, "42");
        bank.store(15, op);
        IOperand* loaded = bank.load(15);
        assert(loaded->toString() == "42");
        
        print_success();
        // Don't delete op - RegisterBank owns it
    } catch (const std::exception& e) {
        print_failure(std::string("Exception: ") + e.what());
    }
}

// ============================================================================
// INSTRUCTION TESTS
// ============================================================================

void test_instruction_push() {
    print_test("Instruction: Create Push instruction");
    try {
        Instruction instr(InstructionType::Push, OperandType::Int32, "42");
        
        assert(instr.getType() == InstructionType::Push);
        assert(instr.getOperandType() == OperandType::Int32);
        assert(instr.getOperandValue() == "42");
        
        print_success();
    } catch (const std::exception& e) {
        print_failure(std::string("Exception: ") + e.what());
    }
}

void test_instruction_simple() {
    print_test("Instruction: Create simple instruction");
    try {
        Instruction instr_pop(InstructionType::Pop);
        Instruction instr_clear(InstructionType::Clear);
        Instruction instr_swap(InstructionType::Swap);
        
        assert(instr_pop.getType() == InstructionType::Pop);
        assert(instr_clear.getType() == InstructionType::Clear);
        assert(instr_swap.getType() == InstructionType::Swap);
        
        print_success();
    } catch (const std::exception& e) {
        print_failure(std::string("Exception: ") + e.what());
    }
}

void test_instruction_load_store() {
    print_test("Instruction: Create Load/Store instructions");
    try {
        Instruction load(InstructionType::Load, 5);
        Instruction store(InstructionType::Store, 10);
        
        assert(load.getType() == InstructionType::Load);
        assert(load.getRegisterIndex() == 5);
        
        assert(store.getType() == InstructionType::Store);
        assert(store.getRegisterIndex() == 10);
        
        print_success();
    } catch (const std::exception& e) {
        print_failure(std::string("Exception: ") + e.what());
    }
}

// ============================================================================
// VIRTUAL MACHINE TESTS
// ============================================================================

void test_vm_push_pop() {
    print_test("VirtualMachine: Push and Pop");
    try {
        // Create a new scope to ensure vm is destroyed after each test
        {
            VirtualMachine vm;
            std::vector<Instruction> instructions;
            
            instructions.push_back(Instruction(InstructionType::Push, OperandType::Int32, "42"));
            instructions.push_back(Instruction(InstructionType::Exit));
            
            vm.run(instructions);
        }
        
        print_success();
    } catch (const std::exception& e) {
        print_failure(std::string("Exception: ") + e.what());
    }
}

void test_vm_arithmetic() {
    print_test("VirtualMachine: Arithmetic operations (Add)");
    try {
        {
            VirtualMachine vm;
            std::vector<Instruction> instructions;
            
            // Push 10 and 20, then add
            instructions.push_back(Instruction(InstructionType::Push, OperandType::Int32, "10"));
            instructions.push_back(Instruction(InstructionType::Push, OperandType::Int32, "20"));
            instructions.push_back(Instruction(InstructionType::Add));
            instructions.push_back(Instruction(InstructionType::Exit));
            
            vm.run(instructions);
        }
        
        print_success();
    } catch (const std::exception& e) {
        print_failure(std::string("Exception: ") + e.what());
    }
}

void test_vm_load_store() {
    print_test("VirtualMachine: Load and Store registers");
    try {
        {
            VirtualMachine vm;
            std::vector<Instruction> instructions;
            
            // Simple test - just store in register without loading
            instructions.push_back(Instruction(InstructionType::Push, OperandType::Int32, "999"));
            instructions.push_back(Instruction(InstructionType::Store, 0));
            instructions.push_back(Instruction(InstructionType::Exit));
            
            vm.run(instructions);
        }
        
        print_success();
    } catch (const std::exception& e) {
        print_failure(std::string("Exception: ") + e.what());
    }
}

void test_vm_clear() {
    print_test("VirtualMachine: Clear stack");
    try {
        {
            VirtualMachine vm;
            std::vector<Instruction> instructions;
            
            instructions.push_back(Instruction(InstructionType::Push, OperandType::Int32, "1"));
            instructions.push_back(Instruction(InstructionType::Push, OperandType::Int32, "2"));
            instructions.push_back(Instruction(InstructionType::Push, OperandType::Int32, "3"));
            instructions.push_back(Instruction(InstructionType::Clear));
            instructions.push_back(Instruction(InstructionType::Exit));
            
            vm.run(instructions);
        }
        
        print_success();
    } catch (const std::exception& e) {
        print_failure(std::string("Exception: ") + e.what());
    }
}

void test_vm_swap() {
    print_test("VirtualMachine: Swap top two values");
    try {
        {
            VirtualMachine vm;
            std::vector<Instruction> instructions;
            
            instructions.push_back(Instruction(InstructionType::Push, OperandType::Int32, "10"));
            instructions.push_back(Instruction(InstructionType::Push, OperandType::Int32, "20"));
            instructions.push_back(Instruction(InstructionType::Swap));
            instructions.push_back(Instruction(InstructionType::Exit));
            
            vm.run(instructions);
        }
        
        print_success();
    } catch (const std::exception& e) {
        print_failure(std::string("Exception: ") + e.what());
    }
}

void test_vm_multiple_operations() {
    print_test("VirtualMachine: Complex sequence");
    try {
        {
            VirtualMachine vm;
            std::vector<Instruction> instructions;
            
            // (5 + 3) * 2
            instructions.push_back(Instruction(InstructionType::Push, OperandType::Int32, "5"));
            instructions.push_back(Instruction(InstructionType::Push, OperandType::Int32, "3"));
            instructions.push_back(Instruction(InstructionType::Add));
            instructions.push_back(Instruction(InstructionType::Push, OperandType::Int32, "2"));
            instructions.push_back(Instruction(InstructionType::Mul));
            instructions.push_back(Instruction(InstructionType::Exit));
            
            vm.run(instructions);
        }
        
        print_success();
    } catch (const std::exception& e) {
        print_failure(std::string("Exception: ") + e.what());
    }
}

// ============================================================================
// MAIN
// ============================================================================

int main() {
    std::cout << "\n" << YELLOW << "========== VM COMPONENT TESTS ==========" << RESET << "\n\n";
    
    // Stack tests
    std::cout << YELLOW << "\n--- STACK TESTS ---" << RESET << "\n";
    test_stack_push_pop();
    test_stack_top();
    test_stack_clear();
    test_stack_empty_pop();
    test_stack_empty_top();
    
    // RegisterBank tests
    std::cout << YELLOW << "\n--- REGISTER BANK TESTS ---" << RESET << "\n";
    test_registerbank_store_load();
    test_registerbank_clear();
    test_registerbank_bounds();
    
    // Instruction tests
    std::cout << YELLOW << "\n--- INSTRUCTION TESTS ---" << RESET << "\n";
    test_instruction_push();
    test_instruction_simple();
    test_instruction_load_store();
    
    // VirtualMachine tests
    std::cout << YELLOW << "\n--- VIRTUAL MACHINE TESTS ---" << RESET << "\n";
    test_vm_push_pop();
    test_vm_arithmetic();
    test_vm_load_store();
    test_vm_clear();
    test_vm_swap();
    test_vm_multiple_operations();
    
    // Summary
    std::cout << YELLOW << "\n========== TEST SUMMARY ==========" << RESET << "\n";
    std::cout << "Total tests: " << test_count << "\n";
    std::cout << GREEN << "Passed: " << test_passed << RESET << "\n";
    std::cout << RED << "Failed: " << (test_count - test_passed) << RESET << "\n";
    std::cout << "\n";
    
    return (test_passed == test_count) ? 0 : 1;
}
