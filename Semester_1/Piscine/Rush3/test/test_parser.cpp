#include <iostream>
#include <sstream>
#include <cassert>
#include "../include/Lexer.hpp"
#include "../include/Parser.hpp"

void runTest(int testNum, const std::string& description, const std::string& input, bool shouldFail = false) {
    std::cout << "=== Test " << testNum << ": " << description << " ===" << std::endl;
    try {
        Lexer lexer(input);
        auto tokens = lexer.tokenize();
        
        Parser parser(tokens);
        auto instructions = parser.parse();
        
        if (shouldFail) {
            std::cout << "FAILED: Should have thrown an error!" << std::endl;
        } else {
            std::cout << "PASSED (Instructions: " << instructions.size() << ")" << std::endl;
        }
    } catch (const std::exception& e) {
        if (shouldFail) {
            std::cout << "PASSED (Correctly caught): " << e.what() << std::endl;
        } else {
            std::cout << "FAILED: " << e.what() << std::endl;
        }
    }
    std::cout << std::endl;
}

int main() {
    // Test 1: Simple push and exit
    runTest(1, "Push int32 42 and exit", R"(
push int32 42
exit
)");
    
    // Test 2: Basic arithmetic
    runTest(2, "Push, push, add, display", R"(
push int32 10
push int32 20
add
display
)");
    
    // Test 3: Float values
    runTest(3, "Float values (float32 and float64)", R"(
push float32 3.14
push float64 2.71828
display
)");
    
    // Test 4: Comments
    runTest(4, "Comments (line and inline)", R"(
; This is a comment
push int32 100 ; inline comment
pop
exit
)");
    
    // Test 5: Load and Store instructions
    runTest(5, "Load/Store registers", R"(
push int32 50
store 0
load 0
display
)");    
    // Test 6: Error - decimal in int32 (should fail)
    runTest(6, "Decimal in int32 (should fail)", R"(
push int32 42.5
)", true);
    
    // Test 7: Assert instruction
    runTest(7, "Assert instruction", R"(
push int32 42
assert int32 42
)");
    
    // Test 8: Arithmetic operations
    runTest(8, "ADD instruction", R"(
push int32 10
push int32 20
add
display
)");
    
    // Test 9: Subtraction
    runTest(9, "SUB instruction", R"(
push int32 50
push int32 20
sub
display
)");
    
    // Test 10: Multiplication
    runTest(10, "MUL instruction", R"(
push int32 6
push int32 7
mul
display
)");
    
    // Test 11: Division
    runTest(11, "DIV instruction", R"(
push int32 100
push int32 5
div
display
)");
    
    // Test 12: Modulo
    runTest(12, "MOD instruction", R"(
push int32 17
push int32 5
mod
display
)");
    
    // Test 13: Swap instruction
    runTest(13, "SWAP instruction", R"(
push int32 10
push int32 20
swap
display
display
)");
    
    // Test 14: Clear instruction
    runTest(14, "CLEAR instruction", R"(
push int32 10
push int32 20
push int32 30
clear
)");
    
    // Test 15: Complex expression (10 + 20) * 2
    runTest(15, "Complex expression - (10 + 20) * 2", R"(
push int32 10
push int32 20
add
push int32 2
mul
display
)");
    
    // Test 16: Negative numbers
    runTest(16, "Negative numbers", R"(
push int32 -42
push int32 10
add
display
)");
    
    // Test 17: Float arithmetic
    runTest(17, "Float arithmetic", R"(
push float32 3.5
push float32 2.5
add
display
)");
    
    // Test 18: Pop instruction
    runTest(18, "POP instruction", R"(
push int32 10
push int32 20
push int32 30
pop
display
)");
    
    // Test 19: All operand types
    runTest(19, "All operand types", R"(
push int8 127
push int16 32767
push int32 2147483647
push int64 9223372036854775807
push float32 3.14
push float64 2.71828
)");
    
    // Test 20: Register 0 boundary
    runTest(20, "Register 0 (boundary)", R"(
push int32 100
store 0
load 0
display
)");
    
    // Test 21: Register 15 boundary
    runTest(21, "Register 15 (boundary)", R"(
push int32 200
store 15
load 15
display
)");
    
    // Test 22: Invalid register (should fail)
    runTest(22, "Invalid register 16 (should fail)", R"(
push int32 100
store 16
)", true);
    
    // Test 23: Invalid register negative (should fail)
    runTest(23, "Invalid negative register (should fail)", R"(
push int32 100
store -1
)", true);
    
    // Test 24: Multiple operations
    runTest(24, "Multiple consecutive operations", R"(
push int32 5
push int32 3
add
push int32 2
mul
display
)");
    
    // Test 25: Empty lines and comments
    runTest(25, "Empty lines and comments", R"(
; Start of program

push int32 10

; Push another value
push int32 20
add

; Display result
display

; End
)");
    
    // Test 26: All int types
    runTest(26, "All integer types", R"(
push int8 100
push int16 1000
push int32 100000
push int64 10000000000
)");
    
    // Test 27: Both float types
    runTest(27, "Both float types", R"(
push float32 1.5
push float64 2.5
add
)");
    
    // Test 28: Mixed int and float (should work - implicit conversion)
    runTest(28, "Mixed int and float operations", R"(
push int32 10
push float32 2.5
add
)");
    
    // Test 29: Exit instruction
    runTest(29, "Exit instruction", R"(
push int32 42
exit
)");
    
    // Test 30: Very large numbers
    runTest(30, "Large numbers", R"(
push int64 9223372036854775807
push int64 1
display
)");
    
    std::cout << "=== All tests completed ===" << std::endl;
    return 0;
}
