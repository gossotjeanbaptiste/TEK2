#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>
#include <memory>
#include <fstream>

enum class TokenType {
    PUSH,
    POP,
    DISPLAY,
    CLEAR,
    SWAP,
    ASSERT,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    LOAD,
    STORE,
    EXIT,
    
    INT8,
    INT16,
    INT32,
    INT64,
    FLOAT32,
    FLOAT64, 
    NUMBER,
    END_OF_LINE,
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token(TokenType t = TokenType::UNKNOWN, const std::string& v = "", int l = 0, int c = 0)
        : type(t), value(v), line(l), column(c) {}
};

class Lexer {
public:
    Lexer(const std::string& input);
    ~Lexer() = default;
    
    std::vector<Token> tokenize();
    Token nextToken();
    void skipComments();
    void skipWhitespace();
    
    // Static methods to load input
    static Lexer fromFile(const std::string& filename);
    static Lexer fromStdin();
    
private:
    std::string input;
    size_t position;
    int line;
    int column;
    
    char peek(int offset = 0) const;
    char advance();
    std::string readWord();
    std::string readNumber();
    TokenType getKeywordType(const std::string& word);
    
    // Helper to read file content
    static std::string readFileContent(const std::string& filename);
    static std::string readStdinContent();
};

#endif
