#ifndef OPERAND_HPP
#define OPERAND_HPP

#include "IOperand.hpp"
#include <cstdint>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <stdexcept>
#include <limits>
#include <type_traits>

class OperandFactory; // Forward declaration

// ============================================================================
// Helper function to determine the more precise type
// ============================================================================
inline OperandType getPreciserType(OperandType type1, OperandType type2)
{
	return (type1 > type2) ? type1 : type2;
}

// ============================================================================
// Base template class for all operand types
// ============================================================================
template <typename T>
class Operand : public IOperand
{
protected:
	T _value;
	OperandType _type;

public:
	Operand(T value, OperandType type) : _value(value), _type(type) {}
	virtual ~Operand() = default;

	OperandType getType() const override { return _type; }
    std::string toString() const override;

	IOperand *operator+(const IOperand &rhs) const override;
	IOperand *operator-(const IOperand &rhs) const override;
	IOperand *operator*(const IOperand &rhs) const override;
	IOperand *operator/(const IOperand &rhs) const override;
	IOperand *operator%(const IOperand &rhs) const override;

	T getValue() const { return _value; }
};

// Concrete operand type aliases
using Int8Operand = Operand<std::int8_t>;
using Int16Operand = Operand<std::int16_t>;
using Int32Operand = Operand<std::int32_t>;
using Int64Operand = Operand<std::int64_t>;
using Float32Operand = Operand<float>;
using Float64Operand = Operand<double>;

// Include template implementations from .cpp
#include "../src/Operand.cpp"

#endif // OPERAND_HPP
