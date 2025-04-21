#pragma once
#include <string>
#include <map>

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
    QUESTION,
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
    BREAK,
    // utils
    EOF_
};

static std::map<TokenType, const char*> token_names {
    {_ERROR, "error"},
    {LITERAL_INT, "lit.int"},
    {LITERAL_STRING, "lit.str"},
    {IDENTIFIER, "id"},
    {PLUS, "plus"},
    {MINUS, "minus"},
    {EQUAL, "eq"},
    {SEMICOLON, "semicol"},
    {COMMA, "comma"},
    {STAR, "star"},
    {SLASH, "slash"},
    {DOT, "dot"},
    {DOT2, "dot2"},
    {LEFT_FIG_BR, "left_fig_br"},
    {LEFT_ROUND_BR, "left_round_br"},
    {RIGHT_FIG_BR, "right_fig_br"},
    {RIGHT_ROUND_BR, "right_round_br"},
    {EQ, "eq"},
    {EQ_EQ, "eq-eq"},
    {GREATER_EQ, "gt-eq"},
    {GREATER, "gt"},
    {LESS, "less"},
    {LESS_EQ, "less_eq"},
    {EXCL, "excl"},
    {NOT_EQ, "neq"},
    {AND, "and"},
    {CLASS, "error"},
    {ELSE, "error"},
    {BOOL_TRUE, "bool.true"},
    {BOOL_FALSE, "bool.false"},
    {FUN, "fun"},
    {FOR, "for"},
    {IF, "if"},
    {NONE, "none"},
    {PRINT, "print"},
    {RETURN, "return"},
    {SUPER, "super"},
    {THIS, "this"},
    {VAR, "var"},
    {WHILE, "while"},
    {QUESTION, "question"},
    {BREAK, "break"},
    // utils
    {EOF_, "eof"}
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

        TokenType get_type() {
            return _type;
        };

        const char* get_name() {
            auto fn = token_names.find(_type);
            if (fn != end(token_names)) {
                return (*fn).second;
            }
            else return "<?>";
        };

};