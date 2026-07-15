#include "../include/OperandFactory.hpp"
#include "../include/Operand.hpp"
#include <stdexcept>
#include <limits>
#include <cstdint>

// Main factory method - dispatches to the correct type creator
IOperand *OperandFactory::createOperand(OperandType type, const std::string &value)
{
	switch (type) {
		case OperandType::Int8:
			return createInt8(value);
		case OperandType::Int16:
			return createInt16(value);
		case OperandType::Int32:
			return createInt32(value);
		case OperandType::Int64:
			return createInt64(value);
		case OperandType::Float32:
			return createFloat32(value);
		case OperandType::Float64:
			return createFloat64(value);
		default:
			throw std::runtime_error("error: unknown operand type.");
	}
}

// Helper function to parse and validate integer values
static std::int64_t parseIntValue(const std::string &value)
{
	try {
		return std::stoll(value);
	} catch (const std::exception &e) {
		throw std::runtime_error("error: invalid integer value: " + value);
	}
}

// Helper function to parse and validate float values
static double parseFloatValue(const std::string &value)
{
	try {
		return std::stod(value);
	} catch (const std::exception &e) {
		throw std::runtime_error("error: invalid float value: " + value);
	}
}

// Generic template to create integer operands with overflow checking
template <typename T>
static IOperand *createIntegerOperand(const std::string &value, OperandType type)
{
	std::int64_t val = parseIntValue(value);
	
	// Validate BEFORE allocating
	if (val > static_cast<std::int64_t>(std::numeric_limits<T>::max()) ||
	    val < static_cast<std::int64_t>(std::numeric_limits<T>::min())) {
		throw std::runtime_error("error: overflow for integer type: " + value);
	}
	
	// Now it's safe to allocate
	return new Operand<T>(static_cast<T>(val), type);
}

// Int8 Creator
IOperand *OperandFactory::createInt8(const std::string &value)
{
	return createIntegerOperand<std::int8_t>(value, OperandType::Int8);
}

// Int16 Creator
IOperand *OperandFactory::createInt16(const std::string &value)
{
	return createIntegerOperand<std::int16_t>(value, OperandType::Int16);
}

// Int32 Creator
IOperand *OperandFactory::createInt32(const std::string &value)
{
	return createIntegerOperand<std::int32_t>(value, OperandType::Int32);
}

// Int64 Creator
IOperand *OperandFactory::createInt64(const std::string &value)
{
	return createIntegerOperand<std::int64_t>(value, OperandType::Int64);
}

// Float32 Creator
IOperand *OperandFactory::createFloat32(const std::string &value)
{
	double val = parseFloatValue(value);
	return new Float32Operand(static_cast<float>(val), OperandType::Float32);
}

// Float64 Creator
IOperand *OperandFactory::createFloat64(const std::string &value)
{
	double val = parseFloatValue(value);
	return new Float64Operand(val, OperandType::Float64);
}
