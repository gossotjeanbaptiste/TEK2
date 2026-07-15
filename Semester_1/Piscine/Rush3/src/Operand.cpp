#ifndef OPERAND_CPP
#define OPERAND_CPP

#include "../include/Operand.hpp"
#include "../include/OperandFactory.hpp"

// toString() implementation
template <typename T>
std::string Operand<T>::toString() const
{
	if constexpr (std::is_integral_v<T>) {
		return std::to_string(static_cast<long long>(_value));
	} else if constexpr (std::is_same_v<T, float>) {
		std::stringstream ss;
		ss << std::fixed << std::setprecision(7) << _value;
		return ss.str();
	} else if constexpr (std::is_same_v<T, double>) {
		std::stringstream ss;
		ss << std::fixed << std::setprecision(15) << _value;
		return ss.str();
	}
}

// Helper: Convert IOperand to double for uniform operations
inline double toDouble(const IOperand &operand)
{
	return std::stod(operand.toString());
}

// Addition: v1 + v2
template <typename T>
IOperand *Operand<T>::operator+(const IOperand &rhs) const
{
	OperandType resultType = getPreciserType(this->getType(), rhs.getType());
	double result = static_cast<double>(_value) + toDouble(rhs);
	return OperandFactory::createOperand(resultType, std::to_string(result));
}

// Subtraction: v2 - v1 (v1 is on top of stack)
template <typename T>
IOperand *Operand<T>::operator-(const IOperand &rhs) const
{
	OperandType resultType = getPreciserType(this->getType(), rhs.getType());
	double result = static_cast<double>(_value) - toDouble(rhs);
	return OperandFactory::createOperand(resultType, std::to_string(result));
}

// Multiplication: v1 * v2
template <typename T>
IOperand *Operand<T>::operator*(const IOperand &rhs) const
{
	OperandType resultType = getPreciserType(this->getType(), rhs.getType());
	double result = static_cast<double>(_value) * toDouble(rhs);
	return OperandFactory::createOperand(resultType, std::to_string(result));
}

// Division: v2 / v1 (v1 is on top of stack - the divisor)
template <typename T>
IOperand *Operand<T>::operator/(const IOperand &rhs) const
{
	double divisor = toDouble(rhs);
	if (divisor == 0)
		throw std::runtime_error("division by zero.");
	OperandType resultType = getPreciserType(this->getType(), rhs.getType());
	double result = static_cast<double>(_value) / divisor;
	return OperandFactory::createOperand(resultType, std::to_string(result));
}

// Modulo: v2 % v1 (v1 is on top of stack - the divisor)
// Note: Only for integer types
template <typename T>
IOperand *Operand<T>::operator%(const IOperand &rhs) const
{
	if constexpr (std::is_floating_point_v<T>) {
		throw std::runtime_error("modulo not supported for float types.");
	} else {
		std::int64_t divisor = std::stoll(rhs.toString());
		if (divisor == 0)
			throw std::runtime_error("modulo by zero.");
		std::int64_t lhs = static_cast<std::int64_t>(_value);
		std::int64_t result = lhs % divisor;
		OperandType resultType = getPreciserType(this->getType(), rhs.getType());
		return OperandFactory::createOperand(resultType, std::to_string(result));
	}
}

#endif // OPERAND_CPP

