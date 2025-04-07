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
    EXCL_EQ,
    // keywords
    AND,
    OR,
    CLASS,
    ELSE,
    BOOL_TRUE,
    FUN,
    FOR,
    IF,
    NONE,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    BOOL_FALSE,
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



};