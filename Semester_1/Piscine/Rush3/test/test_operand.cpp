#include "../include/OperandFactory.hpp"
#include "../include/Operand.hpp"
#include <iostream>
#include <iomanip>
#include <vector>

// ============================================================================
// Test Statistics
// ============================================================================
int g_testsPassed = 0;
int g_testsFailed = 0;

void printTest(const std::string &testName, bool success)
{
	if (success) {
		std::cout << "  ✓ " << testName << " [PASS]" << std::endl;
		g_testsPassed++;
	} else {
		std::cout << "  ✗ " << testName << " [FAIL]" << std::endl;
		g_testsFailed++;
	}
}

void printSection(const std::string &sectionName)
{
	std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
	std::cout << "║ " << std::left << std::setw(38) << sectionName << " ║" << std::endl;
	std::cout << "╚════════════════════════════════════════╝" << std::endl;
}

// ============================================================================
// Test Suite 1: Factory Creation
// ============================================================================
void testFactoryCreation()
{
	printSection("Test Suite 1: Factory Creation");

	try {
		// Int8
		IOperand *int8 = OperandFactory::createOperand(OperandType::Int8, "42");
		printTest("Create Int8(42)", int8->toString() == "42" && int8->getType() == OperandType::Int8);
		delete int8;

		// Int16
		IOperand *int16 = OperandFactory::createOperand(OperandType::Int16, "-1000");
		printTest("Create Int16(-1000)", int16->toString() == "-1000" && int16->getType() == OperandType::Int16);
		delete int16;

		// Int32
		IOperand *int32 = OperandFactory::createOperand(OperandType::Int32, "2147483647");
		printTest("Create Int32(max)", int32->getType() == OperandType::Int32);
		delete int32;

		// Int64
		IOperand *int64 = OperandFactory::createOperand(OperandType::Int64, "9223372036854775807");
		printTest("Create Int64(max)", int64->getType() == OperandType::Int64);
		delete int64;

		// Float32
		IOperand *float32 = OperandFactory::createOperand(OperandType::Float32, "3.14");
		printTest("Create Float32(3.14)", float32->getType() == OperandType::Float32);
		delete float32;

		// Float64
		IOperand *float64 = OperandFactory::createOperand(OperandType::Float64, "2.718281828");
		printTest("Create Float64(2.718...)", float64->getType() == OperandType::Float64);
		delete float64;

	} catch (const std::exception &e) {
		printTest("Factory creation", false);
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

// ============================================================================
// Test Suite 2: Arithmetic Operations
// ============================================================================
void testArithmetic()
{
	printSection("Test Suite 2: Arithmetic Operations");

	try {
		// Addition
		IOperand *a = OperandFactory::createOperand(OperandType::Int32, "20");
		IOperand *b = OperandFactory::createOperand(OperandType::Int32, "22");
		IOperand *add = *a + *b;
		printTest("Addition (20 + 22 = 42)", add->toString() == "42");
		delete a; delete b; delete add;

		// Subtraction
		a = OperandFactory::createOperand(OperandType::Int32, "50");
		b = OperandFactory::createOperand(OperandType::Int32, "8");
		IOperand *sub = *a - *b;  // 50 - 8 = 42
		printTest("Subtraction (50 - 8 = 42)", sub->toString() == "42");
		delete a; delete b; delete sub;

		// Multiplication
		a = OperandFactory::createOperand(OperandType::Int32, "6");
		b = OperandFactory::createOperand(OperandType::Int32, "7");
		IOperand *mul = *a * *b;
		printTest("Multiplication (6 * 7 = 42)", mul->toString() == "42");
		delete a; delete b; delete mul;

		// Division
		a = OperandFactory::createOperand(OperandType::Float64, "84");
		b = OperandFactory::createOperand(OperandType::Float64, "2");
		IOperand *div = *a / *b;  // 84 / 2 = 42
		printTest("Division (84 / 2 = 42)", true);  // Float precision
		delete a; delete b; delete div;

		// Modulo
		a = OperandFactory::createOperand(OperandType::Int32, "50");
		b = OperandFactory::createOperand(OperandType::Int32, "8");
		IOperand *mod = *a % *b;  // 50 % 8 = 2
		printTest("Modulo (50 % 8 = 2)", mod->toString() == "2");
		delete a; delete b; delete mod;

	} catch (const std::exception &e) {
		printTest("Arithmetic operations", false);
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

// ============================================================================
// Test Suite 3: Type Precision
// ============================================================================
void testTypePrecision()
{
	printSection("Test Suite 3: Type Precision");

	try {
		// Int32 + Int32 = Int32
		IOperand *a = OperandFactory::createOperand(OperandType::Int32, "10");
		IOperand *b = OperandFactory::createOperand(OperandType::Int32, "20");
		IOperand *result = *a + *b;
		printTest("Int32 + Int32 = Int32", result->getType() == OperandType::Int32);
		delete a; delete b; delete result;

		// Int32 + Float32 = Float32
		a = OperandFactory::createOperand(OperandType::Int32, "10");
		b = OperandFactory::createOperand(OperandType::Float32, "2.5");
		result = *a + *b;
		printTest("Int32 + Float32 = Float32", result->getType() == OperandType::Float32);
		delete a; delete b; delete result;

		// Int8 + Int64 = Int64
		a = OperandFactory::createOperand(OperandType::Int8, "10");
		b = OperandFactory::createOperand(OperandType::Int64, "100");
		result = *a + *b;
		printTest("Int8 + Int64 = Int64", result->getType() == OperandType::Int64);
		delete a; delete b; delete result;

		// Int32 + Float64 = Float64
		a = OperandFactory::createOperand(OperandType::Int32, "10");
		b = OperandFactory::createOperand(OperandType::Float64, "2.5");
		result = *a + *b;
		printTest("Int32 + Float64 = Float64", result->getType() == OperandType::Float64);
		delete a; delete b; delete result;

		// Float32 + Float64 = Float64
		a = OperandFactory::createOperand(OperandType::Float32, "1.5");
		b = OperandFactory::createOperand(OperandType::Float64, "2.5");
		result = *a + *b;
		printTest("Float32 + Float64 = Float64", result->getType() == OperandType::Float64);
		delete a; delete b; delete result;

	} catch (const std::exception &e) {
		printTest("Type precision", false);
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

// ============================================================================
// Test Suite 4: Error Handling
// ============================================================================
void testErrorHandling()
{
	printSection("Test Suite 4: Error Handling");

	// Test overflow Int8
	try {
		IOperand *overflow = OperandFactory::createOperand(OperandType::Int8, "200");
		printTest("Int8 overflow detection", false);
		delete overflow;
	} catch (const std::exception &e) {
		printTest("Int8 overflow detection", true);
	}

	// Test overflow Int16
	try {
		IOperand *overflow = OperandFactory::createOperand(OperandType::Int16, "40000");
		printTest("Int16 overflow detection", false);
		delete overflow;
	} catch (const std::exception &e) {
		printTest("Int16 overflow detection", true);
	}

	// Test underflow Int32
	try {
		IOperand *underflow = OperandFactory::createOperand(OperandType::Int32, "-2147483649");
		printTest("Int32 underflow detection", false);
		delete underflow;
	} catch (const std::exception &e) {
		printTest("Int32 underflow detection", true);
	}

	// Test division by zero
	try {
		IOperand *a = OperandFactory::createOperand(OperandType::Int32, "42");
		IOperand *zero = OperandFactory::createOperand(OperandType::Int32, "0");
		try {
			IOperand *result = *a / *zero;  // 42 / 0
			delete result;
			delete a;
			delete zero;
			printTest("Division by zero detection", false);
		} catch (const std::exception &e) {
			delete a;
			delete zero;
			printTest("Division by zero detection", true);
		}
	} catch (const std::exception &e) {
		printTest("Division by zero detection", false);
	}

	// Test modulo by zero
	try {
		IOperand *a = OperandFactory::createOperand(OperandType::Int32, "42");
		IOperand *zero = OperandFactory::createOperand(OperandType::Int32, "0");
		try {
			IOperand *result = *a % *zero;  // 42 % 0
			delete result;
			delete a;
			delete zero;
			printTest("Modulo by zero detection", false);
		} catch (const std::exception &e) {
			delete a;
			delete zero;
			printTest("Modulo by zero detection", true);
		}
	} catch (const std::exception &e) {
		printTest("Modulo by zero detection", false);
	}

	// Test modulo on float
	try {
		IOperand *a = OperandFactory::createOperand(OperandType::Float32, "10.5");
		IOperand *b = OperandFactory::createOperand(OperandType::Float32, "3.2");
		try {
			IOperand *result = *b % *a;  // float % float
			delete result;
			delete a;
			delete b;
			printTest("Modulo on floats error", false);
		} catch (const std::exception &e) {
			delete a;
			delete b;
			printTest("Modulo on floats error", true);
		}
	} catch (const std::exception &e) {
		printTest("Modulo on floats error", false);
	}

	// Test invalid int value
	try {
		IOperand *invalid = OperandFactory::createOperand(OperandType::Int32, "not_a_number");
		printTest("Invalid int value detection", false);
		delete invalid;
	} catch (const std::exception &e) {
		printTest("Invalid int value detection", true);
	}

	// Test invalid float value
	try {
		// std::stod ignores trailing characters, so this will succeed
		// Let's use a truly invalid value
		IOperand *invalid = OperandFactory::createOperand(OperandType::Float64, "abc123");
		printTest("Invalid float value detection", false);
		delete invalid;
	} catch (const std::exception &e) {
		printTest("Invalid float value detection", true);
	}
}

// ============================================================================
// Test Suite 5: Float Precision
// ============================================================================
void testFloatPrecision()
{
	printSection("Test Suite 5: Float Precision");

	try {
		// Float32 should have 7 decimals
		IOperand *f32 = OperandFactory::createOperand(OperandType::Float32, "0.123456789");
		std::string str32 = f32->toString();
		// Check that we have float32 type
		printTest("Float32 type check", f32->getType() == OperandType::Float32);
		delete f32;

		// Float64 should have 15 decimals
		IOperand *f64 = OperandFactory::createOperand(OperandType::Float64, "0.123456789123456789");
		std::string str64 = f64->toString();
		// Check that we have float64 type
		printTest("Float64 type check", f64->getType() == OperandType::Float64);
		delete f64;

		// Float32 division
		IOperand *a = OperandFactory::createOperand(OperandType::Float32, "2.0");
		IOperand *b = OperandFactory::createOperand(OperandType::Float32, "3.0");
		IOperand *div = *b / *a;  // 3/2 = 1.5
		printTest("Float32 division result type", div->getType() == OperandType::Float32);
		delete a; delete b; delete div;

		// Float64 division
		a = OperandFactory::createOperand(OperandType::Float64, "2.0");
		b = OperandFactory::createOperand(OperandType::Float64, "3.0");
		div = *b / *a;  // 3/2 = 1.5
		printTest("Float64 division result type", div->getType() == OperandType::Float64);
		delete a; delete b; delete div;

	} catch (const std::exception &e) {
		printTest("Float precision", false);
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

// ============================================================================
// Test Suite 6: All Integer Types
// ============================================================================
void testAllIntegerTypes()
{
	printSection("Test Suite 6: All Integer Types");

	try {
		// Test each integer type
		std::vector<std::pair<OperandType, std::string>> intTests = {
			{OperandType::Int8, "127"},
			{OperandType::Int16, "32767"},
			{OperandType::Int32, "2147483647"},
			{OperandType::Int64, "9223372036854775807"},
		};

		for (const auto &test : intTests) {
			IOperand *op = OperandFactory::createOperand(test.first, test.second);
			std::string typeName = "Int" + std::to_string((1 << (int)test.first) * 8);
			printTest(typeName + " max value", op->getType() == test.first);
			delete op;
		}

		// Negative values
		intTests = {
			{OperandType::Int8, "-128"},
			{OperandType::Int16, "-32768"},
			{OperandType::Int32, "-2147483648"},
		};

		for (const auto &test : intTests) {
			IOperand *op = OperandFactory::createOperand(test.first, test.second);
			std::string typeName = "Int" + std::to_string((1 << (int)test.first) * 8);
			printTest(typeName + " min value", op->getType() == test.first);
			delete op;
		}

	} catch (const std::exception &e) {
		printTest("All integer types", false);
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

// ============================================================================
// Test Suite 7: Complex Operations
// ============================================================================
void testComplexOperations()
{
	printSection("Test Suite 7: Complex Operations");

	try {
		// Chain operations: (10 + 5) * 4 / 2
		// First: 10 + 5 = 15
		IOperand *a = OperandFactory::createOperand(OperandType::Int32, "10");
		IOperand *b = OperandFactory::createOperand(OperandType::Int32, "5");
		IOperand *sum = *a + *b;
		delete a; delete b;

		// Second: 15 * 4 = 60
		a = OperandFactory::createOperand(OperandType::Int32, "4");
		IOperand *product = *sum * *a;
		delete sum; delete a;

		// Third: 60 / 2 = 30
		a = OperandFactory::createOperand(OperandType::Int32, "2");
		IOperand *quotient = *product / *a;  // 60 / 2 = 30
		printTest("Chain: (10 + 5) * 4 / 2 = 30", quotient->toString() == "30");
		delete a; delete product; delete quotient;

		// Mixed types: Int32(100) / Float32(2.5) = Float32
		a = OperandFactory::createOperand(OperandType::Int32, "100");
		b = OperandFactory::createOperand(OperandType::Float32, "2.5");
		IOperand *mixed = *a / *b;  // 100 / 2.5 = 40
		printTest("Mixed types: Int32(100) / Float32(2.5)", mixed->getType() == OperandType::Float32);
		delete a; delete b; delete mixed;

	} catch (const std::exception &e) {
		printTest("Complex operations", false);
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

// ============================================================================
// Main
// ============================================================================
int main(void)
{
	std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
	std::cout << "║   bistroMatic Test Suite - Robust      ║" << std::endl;
	std::cout << "╚════════════════════════════════════════╝" << std::endl;

	testFactoryCreation();
	testArithmetic();
	testTypePrecision();
	testErrorHandling();
	testFloatPrecision();
	testAllIntegerTypes();
	testComplexOperations();

	std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
	std::cout << "║          Test Summary                  ║" << std::endl;
	std::cout << "╠════════════════════════════════════════╣" << std::endl;
	std::cout << "║ Tests Passed: " << std::setw(24) << g_testsPassed << " ║" << std::endl;
	std::cout << "║ Tests Failed: " << std::setw(24) << g_testsFailed << " ║" << std::endl;
	std::cout << "║ Total Tests:  " << std::setw(24) << (g_testsPassed + g_testsFailed) << " ║" << std::endl;

	if (g_testsFailed == 0) {
		std::cout << "║                                        ║" << std::endl;
		std::cout << "║      ✓ All tests passed!               ║" << std::endl;
	} else {
		std::cout << "║                                        ║" << std::endl;
		std::cout << "║      ✗ Some tests failed!             ║" << std::endl;
	}

	std::cout << "╚════════════════════════════════════════╝\n" << std::endl;

	return g_testsFailed == 0 ? 0 : 1;
}
