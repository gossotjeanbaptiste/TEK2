#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include <memory>
#include "Lexer.hpp"
#include "IOperand.hpp"
#include "InstructionExecutor.hpp"

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    ~Parser() = default;
    
    std::vector<Instruction> parse();
    
private:
    std::vector<Token> tokens;
    size_t position;
    
    Token peek(int offset = 0) const;
    Token advance();
    bool match(TokenType type);
    
    Instruction parseInstruction();
    std::shared_ptr<IOperand> parseValue();
    std::pair<OperandType, std::string> parsePushValue();
    void skipEndOfLines();
    bool isValidNumberForType(TokenType type, const std::string& value);
};

#endif // PARSER_HPP
