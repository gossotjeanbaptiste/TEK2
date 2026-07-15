#ifndef OPERANDFACTORY_HPP
#define OPERANDFACTORY_HPP

#include "IOperand.hpp"
#include <string>

class OperandFactory
{
public:
	// Static method to create operands of any type
	// Takes an OperandType enum and a string value
	static IOperand *createOperand(OperandType type, const std::string &value);

private:
	OperandFactory(); // Private constructor - cannot instantiate

	// Private factory methods for each type
	static IOperand *createInt8(const std::string &value);
	static IOperand *createInt16(const std::string &value);
	static IOperand *createInt32(const std::string &value);
	static IOperand *createInt64(const std::string &value);
	static IOperand *createFloat32(const std::string &value);
	static IOperand *createFloat64(const std::string &value);
};

#endif // OPERANDFACTORY_HPP
