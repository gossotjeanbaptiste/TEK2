#include "../include/Lexer.hpp"
#include <cctype>
#include <fstream>
#include <iostream>

Lexer::Lexer(const std::string& input)
    : input(input), position(0), line(1), column(1) {}

char Lexer::peek(int offset) const {
    if (position + offset >= input.length())
        return '\0';
    return input[position + offset];
}

char Lexer::advance() {
    if (position >= input.length())
        return '\0';
    
    char ch = input[position++];
    if (ch == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return ch;
}

void Lexer::skipComments() {
    if (peek() == ';') {
        while (peek() != '\n' && peek() != '\0') {
            advance();
        }
    }
}

void Lexer::skipWhitespace() {
    while (peek() != '\0' && std::isspace(peek()) && peek() != '\n') {
        advance();
    }
}

std::string Lexer::readWord() {
    std::string word;
    while (peek() != '\0' && (std::isalnum(peek()) || peek() == '_')) {
        word += advance();
    }
    return word;
}

std::string Lexer::readNumber() {
    std::string number;
    bool hasDecimal = false;
    
    
    if (peek() == '-' || peek() == '+') {
        number += advance();
    }
    while (peek() != '\0' && (std::isdigit(peek()) || peek() == '.')) {
        if (peek() == '.') {
            if (hasDecimal) break;
            hasDecimal = true;
        }
        number += advance();
    }
    
    return number;
}

TokenType Lexer::getKeywordType(const std::string& word) {
    if (word == "push") return TokenType::PUSH;
    if (word == "pop") return TokenType::POP;
    if (word == "display") return TokenType::DISPLAY;
    if (word == "clear") return TokenType::CLEAR;
    if (word == "swap") return TokenType::SWAP;
    if (word == "assert") return TokenType::ASSERT;
    if (word == "add") return TokenType::ADD;
    if (word == "sub") return TokenType::SUB;
    if (word == "mul") return TokenType::MUL;
    if (word == "div") return TokenType::DIV;
    if (word == "mod") return TokenType::MOD;
    if (word == "load") return TokenType::LOAD;
    if (word == "store") return TokenType::STORE;
    if (word == "exit") return TokenType::EXIT;

    if (word == "int8") return TokenType::INT8;
    if (word == "int16") return TokenType::INT16;
    if (word == "int32") return TokenType::INT32;
    if (word == "int64") return TokenType::INT64;
    if (word == "float32") return TokenType::FLOAT32;
    if (word == "float64") return TokenType::FLOAT64;
    
    return TokenType::UNKNOWN;
}

Token Lexer::nextToken() {
    skipWhitespace();
    skipComments();
    skipWhitespace();
    
    if (peek() == '\0')
        return Token(TokenType::END_OF_FILE, "", line, column);
    
    if (peek() == '\n') {
        advance();
        return Token(TokenType::END_OF_LINE, "\n", line - 1, column);
    }
    
    int startLine = line;
    int startColumn = column;
    

    if (std::isalpha(peek()) || peek() == '_') {
        std::string word = readWord();
        TokenType type = getKeywordType(word);
        return Token(type, word, startLine, startColumn);
    }
    
    if (std::isdigit(peek()) || (peek() == '-' && std::isdigit(peek(1)))) {
        std::string number = readNumber();
        return Token(TokenType::NUMBER, number, startLine, startColumn);
    }
    
    if ((peek() == '-' || peek() == '+') && std::isdigit(peek(1))) {
        std::string number = readNumber();
        return Token(TokenType::NUMBER, number, startLine, startColumn);
    }
    
    char unknown = advance();
    return Token(TokenType::UNKNOWN, std::string(1, unknown), startLine, startColumn);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (true) {
        Token token = nextToken();
        
        if (token.type != TokenType::END_OF_LINE || tokens.empty() 
            || tokens.back().type != TokenType::END_OF_LINE) {
            tokens.push_back(token);
        }
        
        if (token.type == TokenType::END_OF_FILE)
            break;
    }
    
    return tokens;
}

std::string Lexer::readFileContent(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    std::string content;
    std::string line;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    
    return content;
}

std::string Lexer::readStdinContent() {
    std::string line;
    std::string content;
    
    while (std::getline(std::cin, line)) {
        content += line + "\n";
    }
    
    return content;
}

Lexer Lexer::fromFile(const std::string& filename) {
    std::string content = readFileContent(filename);
    return Lexer(content);
}

Lexer Lexer::fromStdin() {
    std::string content = readStdinContent();
    return Lexer(content);
}
