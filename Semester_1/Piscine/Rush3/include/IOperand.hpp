#ifndef IOPERAND_HPP
#define IOPERAND_HPP

#include <string>
#include <memory>

enum class OperandType {
	Int8,
	Int16,
	Int32,
	Int64,
	Float32,
	Float64
};

class IOperand
{
public:
	virtual ~IOperand() = default;
	virtual std::string toString() const = 0; // get the value convert in string
	virtual OperandType getType() const = 0; // get the type of the operand
	virtual IOperand *operator+(const IOperand &rhs) const = 0;
	virtual IOperand *operator-(const IOperand &rhs) const = 0;
	virtual IOperand *operator*(const IOperand &rhs) const = 0;
	virtual IOperand *operator/(const IOperand &rhs) const = 0;
	virtual IOperand *operator%(const IOperand &rhs) const = 0;
};

#endif /* IOPERAND_HPP */
