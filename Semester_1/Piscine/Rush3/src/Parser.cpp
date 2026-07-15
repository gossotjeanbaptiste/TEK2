#include "../include/Parser.hpp"
#include "../include/OperandFactory.hpp"
#include "../include/IOperand.hpp"
#include <stdexcept>
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens)
    : tokens(tokens), position(0) {}

Token Parser::peek(int offset) const {
    if (position + offset >= tokens.size())
        return Token(TokenType::END_OF_FILE, "", 0, 0);
    return tokens[position + offset];
}

Token Parser::advance() {
    if (position >= tokens.size())
        return Token(TokenType::END_OF_FILE, "", 0, 0);
    return tokens[position++];
}

bool Parser::match(TokenType type) {
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

void Parser::skipEndOfLines() {
    while (match(TokenType::END_OF_LINE)) {}
        // Skip
}

bool Parser::isValidNumberForType(TokenType type, const std::string& value) {
    bool hasDecimalPoint = value.find('.') != std::string::npos;
    if ((type == TokenType::INT8 || type == TokenType::INT16 ||
        type == TokenType::INT32 || type == TokenType::INT64)) {
        if (hasDecimalPoint) {
            return false;
        }
    }
    return true;
}

std::pair<OperandType, std::string> Parser::parsePushValue() {
    skipEndOfLines();
    Token typeToken = advance();
    TokenType type = typeToken.type;
    if (type != TokenType::INT8 && type != TokenType::INT16 &&
        type != TokenType::INT32 && type != TokenType::INT64 &&
        type != TokenType::FLOAT32 && type != TokenType::FLOAT64)
        throw std::runtime_error("Expected operand type (int8, int16, int32, int64, float32, float64) at line " 
                                + std::to_string(typeToken.line));
    skipEndOfLines();
    Token valueToken = advance();
    if (valueToken.type != TokenType::NUMBER)
        throw std::runtime_error("Expected number value at line " + std::to_string(valueToken.line));
    if (!isValidNumberForType(type, valueToken.value))
        throw std::runtime_error("Invalid number format for type at line " + std::to_string(valueToken.line) +
                                ": integer type cannot have decimal point");
    OperandType operandType;
    switch (type) {
        case TokenType::INT8: operandType = OperandType::Int8; break;
        case TokenType::INT16: operandType = OperandType::Int16; break;
        case TokenType::INT32: operandType = OperandType::Int32; break;
        case TokenType::INT64: operandType = OperandType::Int64; break;
        case TokenType::FLOAT32: operandType = OperandType::Float32; break;
        case TokenType::FLOAT64: operandType = OperandType::Float64; break;
        default: throw std::runtime_error("Invalid operand type");
    }
    return {operandType, valueToken.value};
}

std::shared_ptr<IOperand> Parser::parseValue() {
    skipEndOfLines();
    
    Token typeToken = advance();
    TokenType type = typeToken.type;
    
    if (type != TokenType::INT8 && type != TokenType::INT16 &&
        type != TokenType::INT32 && type != TokenType::INT64 &&
        type != TokenType::FLOAT32 && type != TokenType::FLOAT64)
        throw std::runtime_error("Expected operand type (int8, int16, int32, int64, float32, float64) at line " 
                                + std::to_string(typeToken.line));
    skipEndOfLines();
    Token valueToken = advance();
    if (valueToken.type != TokenType::NUMBER)
        throw std::runtime_error("Expected number value at line " + std::to_string(valueToken.line));
    if (!isValidNumberForType(type, valueToken.value))
        throw std::runtime_error("Invalid number format for type at line " + std::to_string(valueToken.line) +
                                ": integer type cannot have decimal point");
    OperandType operandType;
    switch (type) {
        case TokenType::INT8: operandType = OperandType::Int8; break;
        case TokenType::INT16: operandType = OperandType::Int16; break;
        case TokenType::INT32: operandType = OperandType::Int32; break;
        case TokenType::INT64: operandType = OperandType::Int64; break;
        case TokenType::FLOAT32: operandType = OperandType::Float32; break;
        case TokenType::FLOAT64: operandType = OperandType::Float64; break;
        default: throw std::runtime_error("Invalid operand type");
    }
    IOperand* rawOperand = OperandFactory::createOperand(operandType, valueToken.value);
    return std::shared_ptr<IOperand>(rawOperand);
}

Instruction Parser::parseInstruction() {
    skipEndOfLines();
    if (peek().type == TokenType::END_OF_FILE)
        return Instruction(InstructionType::Exit);
    
    Token instrToken = advance();
    switch (instrToken.type) {
        case TokenType::PUSH: {
            auto [operandType, value] = parsePushValue();
            return Instruction(InstructionType::Push, operandType, value);
        }
        case TokenType::POP:
            return Instruction(InstructionType::Pop);
        case TokenType::DISPLAY:
            return Instruction(InstructionType::Display);
        case TokenType::CLEAR:
            return Instruction(InstructionType::Clear);
        case TokenType::SWAP:
            return Instruction(InstructionType::Swap);
        case TokenType::ASSERT: {
            auto [operandType, value] = parsePushValue();
            return Instruction(InstructionType::Assert, operandType, value);
        }
        case TokenType::ADD:
            return Instruction(InstructionType::Add);
        case TokenType::SUB:
            return Instruction(InstructionType::Sub);
        case TokenType::MUL:
            return Instruction(InstructionType::Mul);
        case TokenType::DIV:
            return Instruction(InstructionType::Div);
        case TokenType::MOD:
            return Instruction(InstructionType::Mod);
        case TokenType::LOAD: {
            skipEndOfLines();
            Token numToken = advance();
            if (numToken.type != TokenType::NUMBER)
                throw std::runtime_error("Expected register number for LOAD at line " + std::to_string(numToken.line));
            int regNum = std::stoi(numToken.value);
            if (regNum < 0 || regNum > 15)
                throw std::runtime_error("Register number must be between 0 and 15 at line " + std::to_string(numToken.line));
            return Instruction(InstructionType::Load, regNum);
        }
        case TokenType::STORE: {
            skipEndOfLines();
            Token numToken = advance();
            if (numToken.type != TokenType::NUMBER)
                throw std::runtime_error("Expected register number for STORE at line " + std::to_string(numToken.line));
            int regNum = std::stoi(numToken.value);
            if (regNum < 0 || regNum > 15)
                throw std::runtime_error("Register number must be between 0 and 15 at line " + std::to_string(numToken.line));
            return Instruction(InstructionType::Store, regNum);
        }
        case TokenType::EXIT:
            return Instruction(InstructionType::Exit);
        default:
            throw std::runtime_error("Unknown instruction: " + instrToken.value + " at line " + std::to_string(instrToken.line));
    }
}

std::vector<Instruction> Parser::parse() {
    std::vector<Instruction> instructions;

    while (peek().type != TokenType::END_OF_FILE) {
        skipEndOfLines();
        if (peek().type == TokenType::END_OF_FILE)
            break;
        instructions.push_back(parseInstruction());
        skipEndOfLines();
    }
    return instructions;
}
