#ifndef COMPILER_SCANNER_H
#define COMPILER_SCANNER_H

#include "string.h"
#include "common.h"

typedef enum {
    // single char
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
    TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
    TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,
    // one or two characters
    TOKEN_BANG, TOKEN_BANG_EQUAL,
    TOKEN_EQ, TOKEN_EQ_EQ,
    TOKEN_GT, TOKEN_LESS, TOKEN_GT_EQ, TOKEN_LESS_EQ,
    // literals
    TOKEN_ID, TOKEN_STRING, TOKEN_NUMBER,
    // keywords
    TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE,
    TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NULL, TOKEN_OR,
    TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS,
    TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE,
    // mods
    TOKEN_LET,
    TOKEN_SWITCH,
    TOKEN_DEFAULT,
    TOKEN_CASE,
    TOKEN_BREAK,
    TOKEN_COLON,

    TOKEN_ERROR, TOKEN_EOF
} TOKEN_TYPE;

typedef struct {
    TOKEN_TYPE type;
    int line;
    int length;
    const char* start;
} Token;

void scanner_init(const char* source);
Token scan_token();

#endif