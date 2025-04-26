#include "compiler.h"

typedef struct {
    Token current;
    Token previous;
    bool had_error;
    bool panic_mode;
} Parser;

typedef enum {
    PREC_NONE,
    PREC_ASSIGMENT, // =
    PREC_OR, // or
    PREC_AND, // and
    PREC_EQUALITY, // == !=
    PREC_COMPARISON, // < > <= >=
    PREC_TERM, // + -
    PREC_FACTOR, // * /
    PREC_UNARY, // ! -
    PREC_CALL, // .()
    PREC_PRIMARY,
} PrecedenceOrder;

typedef void (*ParseFunc)();

typedef struct  {
    ParseFunc prefix;
    ParseFunc infix;
    PrecedenceOrder prec;
} ParseRule;

Parser parser;
Chunk* compiling_chunk;

void error_at(Token* token, const char* msg) {
    fprintf(stderr, "[line %d] Error", token->line);

    if (parser.panic_mode) return;
    parser.panic_mode = true;

    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type == TOKEN_ERROR) {
        // nothing
    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", msg);
    parser.had_error = true;
} 

void error(const char* msg) {
    error_at(&parser.previous, msg);
}

void error_at_current(const char* msg) {
    error_at(&parser.current, msg);
}

void consume(TOKEN_TYPE type, const char* msg) {
    if (parser.current.type == type) {
        advance();
        return;
    }

    error_at_current(msg);
};


// ------------ CHUNK TOOLS

Chunk* current_chunk() {
    return compiling_chunk;
};

uint8_t make_constant(Value value) {
    int const_index = chunk_add_constant(current_chunk(), value);
    if (const_index > UINT8_MAX) {
        error("too many constants in one chunk.");
        return 0;
    }

    return (uint8_t)const_index;
};


// ------------ EMITTING BYTECODE

void emit_byte(uint8_t byte) {
    chunk_write(current_chunk(), byte, parser.previous.line);
};

void emit_op_return() {
    emit_byte(OP_RET);
};

void emit_bytes(uint8_t byte1, uint8_t byte2) {
    emit_byte(byte1);
    emit_byte(byte2);
};

void emit_constant(Value value) {
    emit_bytes(OP_CONST, make_constant(value));
}

void end_compiler() {
    emit_op_return();
};

// -------------------- PARSING

void comp_number() {
    double v = strtod(parser.previous.start, NULL);
    emit_constant(v);
};

void unary();
void binary();
void grouping();


ParseRule rules[] = {
      [TOKEN_LEFT_PAREN]    = {grouping,    NULL,   PREC_NONE},
      [TOKEN_RIGHT_PAREN]   = {NULL,        NULL,   PREC_NONE},
      [TOKEN_LEFT_BRACE]    = {NULL,        NULL,   PREC_NONE},
      [TOKEN_RIGHT_BRACE]   = {NULL,        NULL,   PREC_NONE},
      [TOKEN_COMMA]         = {NULL,        NULL,   PREC_NONE},
      [TOKEN_DOT]           = {NULL,        NULL,   PREC_NONE},
      [TOKEN_MINUS]         = {unary,       binary, PREC_TERM},
      [TOKEN_PLUS]          = {NULL,        binary, PREC_TERM},
      [TOKEN_SEMICOLON]     = {NULL,        NULL,   PREC_NONE},
      [TOKEN_SLASH]         = {NULL,        binary, PREC_FACTOR},
      [TOKEN_STAR]          = {NULL,        binary, PREC_FACTOR},
      [TOKEN_BANG]          = {NULL,        NULL,   PREC_NONE},
      [TOKEN_BANG_EQUAL]    = {NULL,        NULL,   PREC_NONE},
      [TOKEN_EQ]            = {NULL,        NULL,   PREC_NONE},
      [TOKEN_EQ_EQ]         = {NULL,        NULL,   PREC_NONE},
      [TOKEN_GT]            = {NULL,        NULL,   PREC_NONE},
      [TOKEN_GT_EQ]         = {NULL,        NULL,   PREC_NONE},
      [TOKEN_LESS]          = {NULL,        NULL,   PREC_NONE},
      [TOKEN_LESS_EQ]       = {NULL,        NULL,   PREC_NONE},
      [TOKEN_ID]            = {NULL,        NULL,   PREC_NONE},
      [TOKEN_STRING]        = {NULL,        NULL,   PREC_NONE},
      [TOKEN_NUMBER]        = {comp_number,      NULL,   PREC_NONE},
      [TOKEN_AND]           = {NULL,        NULL,   PREC_NONE},
      [TOKEN_CLASS]         = {NULL,        NULL,   PREC_NONE},
      [TOKEN_ELSE]          = {NULL,        NULL,   PREC_NONE},
      [TOKEN_FALSE]         = {NULL,        NULL,   PREC_NONE},
      [TOKEN_FOR]           = {NULL,        NULL,   PREC_NONE},
      [TOKEN_FUN]           = {NULL,        NULL,   PREC_NONE},
      [TOKEN_IF]            = {NULL,        NULL,   PREC_NONE},
      [TOKEN_NULL]          = {NULL,        NULL,   PREC_NONE},
      [TOKEN_OR]            = {NULL,        NULL,    PREC_OR},
      [TOKEN_PRINT]         = {NULL,        NULL,   PREC_NONE},
      [TOKEN_RETURN]        = {NULL,        NULL,   PREC_NONE},
      [TOKEN_SUPER]         = {NULL,        NULL,   PREC_NONE},
      [TOKEN_THIS]          = {NULL,        NULL,   PREC_NONE},
      [TOKEN_TRUE]          = {NULL,        NULL,   PREC_NONE},
      [TOKEN_VAR]           = {NULL,        NULL,   PREC_NONE},
      [TOKEN_WHILE]         = {NULL,        NULL,   PREC_NONE},
      [TOKEN_ERROR]         = {NULL,        NULL,   PREC_NONE},
      [TOKEN_EOF]           = {NULL,        NULL,   PREC_NONE},
};

ParseRule* get_rule(TOKEN_TYPE type) {
    return &rules[type];
};

void parse_precedence(PrecedenceOrder prec) {
    advance();

    ParseFunc prefix = get_rule(parser.previous.type)->prefix;
    if (prefix == NULL) {
        error("Expect expression.");
        return;
    }

    prefix();

    while(prec <= get_rule(parser.current.type)->prec) {
        advance();
        ParseFunc infix = get_rule(parser.previous.type)->infix;
        infix();
    }
};

void unary() {
    // -a

    TOKEN_TYPE operator = parser.previous.type;
    
    // compile operand
    parse_precedence(PREC_UNARY);
    //expression();
 
    switch (operator)
    {
        case TOKEN_MINUS: emit_byte(OP_NEGATE); break;
        default: return;
    }
};

void binary() {
    TOKEN_TYPE operator = parser.previous.type;
    ParseRule* rule = get_rule(operator);
    parse_precedence((PrecedenceOrder)(rule->prec + 1));

    switch (operator)
    {
    case TOKEN_PLUS: emit_byte(OP_ADD); break;
    case TOKEN_MINUS: emit_byte(OP_SUB); break;
    case TOKEN_STAR: emit_byte(OP_MUL); break;
    case TOKEN_SLASH: emit_byte(OP_DIV); break;
    default:
        return;
    }
};

void grouping() {
    // ( expr )
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
};




void expression() {
    parse_precedence(PREC_ASSIGMENT);
};



void advance() {
    parser.previous = parser.current;
    
    for (;;) {
        parser.current = scan_token();
        if (parser.current.type != TOKEN_ERROR) break;

        error_at_current(parser.current.start);
    }
}

bool compile(const char* source, Chunk* chunk) {
    scanner_init(source);

    compiling_chunk = chunk;
    parser.had_error = false;
    parser.panic_mode = false;

    advance();
    expression();
    consume(TOKEN_EOF, "Expect end of expression.");
    end_compiler();

    return !parser.had_error;
};

void dump_pass() {
    int line = -1;
    for (;;) {
        Token tok = scan_token();
        if (tok.line != line) {
            printf("%4d ", tok.line);
            line = tok.line;
        }
        else {
            printf("    | ");
        }

        printf("%2d '%.*s'\n", tok.type, tok.length, tok.start);
        if (tok.type == TOKEN_EOF) break;
    }
}