#pragma once
#include <string>

enum TokenType {
    _ERROR,
    // lits
    LITERAL_INT,
    LITERAL_STRING,
    IDENTIFIER,
    // math
    PLUS,
    MINUS,
    EQUAL,
    SEMICOLON,
    COMMA,
    STAR,
    SLASH,
    DOT,
    DOT2,
    // bracers
    LEFT_ROUND_BR,
    LEFT_FIG_BR,
    RIGHT_ROUND_BR,
    RIGHT_FIG_BR,
    // eq
    EQ,
    EQ_EQ,
    GREATER_EQ,
    GREATER,
    LESS,
    LESS_EQ,
    EXCL, // !
    NOT_EQ,
    // keywords
    AND,
    OR,
    CLASS,
    ELSE,
    BOOL_TRUE,
    BOOL_FALSE,
    FUN,
    FOR,
    IF,
    NONE,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    VAR,
    WHILE,
    // utils
    EOF_
};

class Token {
    private:
        std::string _lex;
        TokenType _type;
        void* _literal;
        int _line;  

    public:
        Token(std::string lex, TokenType type, void* literal, int line) 
        : _lex(lex), _type(type), _literal(literal), _line(line){};

        std::string get_lex() {
            return _lex;
        };

};