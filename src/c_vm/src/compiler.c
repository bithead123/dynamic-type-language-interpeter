#include "compiler.h"
#include "debug.h"
#include "object.h"
#include "string.h"

#define UINT8_COUNT (UINT8_MAX + 1)

typedef struct {
    int depth;
    Token name;
} Local;

typedef struct {
    int local_count;
    int scope_depth;
    Local locals[UINT8_COUNT];
} Compiler;

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

typedef void (*ParseFunc)(bool canAssign);

typedef struct  {
    ParseFunc prefix;
    ParseFunc infix;
    PrecedenceOrder prec;
} ParseRule;

Parser parser;
Compiler* current_comp = NULL;
Chunk* compiling_chunk;
Hashtable string_constants;

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

bool check(TOKEN_TYPE type) {
    return parser.current.type == type;
};

bool match_token(TOKEN_TYPE type) {
    if (!check(type)) return false;
    advance();
    return true;
};

// ---- Compiler tools
// popping local variables after scope end.
void shrink_local_variables() {
    while(current_comp->local_count > 0 &&
        current_comp->locals[current_comp->local_count-1].depth >
        current_comp->scope_depth) {
            COMPILER_DEBUG_LOG("shrink_local_variables\n");
            emit_byte(OP_POP);
            current_comp->local_count--;
        }
};

bool identifier_equal(Token* a, Token* b) {
    if (a->length != b->length) return false;
    return (strncmp(a->start, b->start, a->length) == 0);
};

// try to find local variable and return they index.
int resolve_local(Compiler* comp, Token* name) {
    for (int i = comp->local_count - 1; i >= 0; i--) {
        Local* local = &comp->locals[i];
        if (identifier_equal(name, &local->name)) {
            return i;
        }
    }

    return -1;
}

void scope_begin() {
    current_comp->scope_depth++;
};

void scope_end() {
    current_comp->scope_depth--;
    shrink_local_variables();
};

void add_local(Token name) {
    
    if (current_comp->local_count == UINT8_COUNT) {
        error("Too many local variables in function.");
        return;
    }

    Local* local = &current_comp->locals[current_comp->local_count++];
    local->name = name;
    local->depth = current_comp->scope_depth;
};

// ------------ CHUNK TOOLS

Chunk* current_chunk() {
    return compiling_chunk;
};

uint8_t make_constant(Value reprValue) {
    int const_index = chunk_add_constant(current_chunk(), reprValue);

    if (const_index > UINT8_MAX) {
        error("too many constants in one chunk.");
        return 0;
    }

    Value vg = compiling_chunk->constants.values[const_index];

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

void comp_number(bool canAssign) {
    double v = strtod(parser.previous.start, NULL);
    Value vv = NUMBER_VAL(v);
    emit_constant(vv);
};

void comp_string(bool canAssign) {
    emit_constant(OBJ_VAL(copy_string(
        parser.previous.start+1, parser.previous.length-2)));
};

uint8_t make_id_constant(Token* name) {
    
    ObjString* str = copy_string(name->start, name->length);
    Value index;
    if (hashtable_get(&string_constants, str, &index)) {
        return (uint8_t)AS_NUMBER(index);
    }
   
    uint8_t const_index = make_constant(OBJ_VAL(str));
    hashtable_set(&string_constants, str, NUMBER_VAL((double)const_index));
    return const_index;
};


void declare_variable() {
    // skip global decls
    if (current_comp->scope_depth == 0) return;

    Token* name = &parser.previous;

    // check var in same scope
    for (int i = current_comp->local_count -1; i >= 0; i--) {
        Local* local = &current_comp->locals[i];
        if (local->depth != -1 && local->depth < current_comp->scope_depth) {
            break;
        }

        if (identifier_equal(name, &local->name)) {
            error("Already a variable with this name in this scope.");
        }
    }

    add_local(*name);
};

uint8_t parse_variable(const char* errorMsg) {
    consume(TOKEN_ID, errorMsg);

    declare_variable();
    if (current_comp->scope_depth > 0) return 0;

    return make_id_constant(&parser.previous);
};

void unary(bool canAssign);
void binary();
void grouping(bool canAssign);
void literal(bool canAssign);
void variable(bool canAssign);

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
      [TOKEN_BANG]          = {unary,        NULL,   PREC_NONE},
      [TOKEN_BANG_EQUAL]    = {NULL,        binary,   PREC_EQUALITY},
      [TOKEN_EQ]            = {NULL,        NULL,   PREC_NONE},
      [TOKEN_EQ_EQ]         = {NULL,        binary,   PREC_EQUALITY},
      [TOKEN_GT]            = {NULL,        binary,   PREC_COMPARISON},
      [TOKEN_GT_EQ]         = {NULL,        binary,   PREC_COMPARISON},
      [TOKEN_LESS]          = {NULL,        binary,   PREC_COMPARISON},
      [TOKEN_LESS_EQ]       = {NULL,        binary,   PREC_COMPARISON},
      [TOKEN_ID]            = {variable,        NULL,   PREC_NONE},
      [TOKEN_STRING]        = {comp_string, NULL,   PREC_NONE},
      [TOKEN_NUMBER]        = {comp_number, NULL,   PREC_NONE},
      [TOKEN_AND]           = {NULL,        NULL,   PREC_NONE},
      [TOKEN_CLASS]         = {NULL,        NULL,   PREC_NONE},
      [TOKEN_ELSE]          = {NULL,        NULL,   PREC_NONE},
      [TOKEN_FALSE]         = {literal,        NULL,   PREC_NONE},
      [TOKEN_FOR]           = {NULL,        NULL,   PREC_NONE},
      [TOKEN_FUN]           = {NULL,        NULL,   PREC_NONE},
      [TOKEN_IF]            = {NULL,        NULL,   PREC_NONE},
      [TOKEN_NULL]          = {literal,        NULL,   PREC_NONE},
      [TOKEN_OR]            = {NULL,        NULL,    PREC_OR},
      [TOKEN_PRINT]         = {NULL,        NULL,   PREC_NONE},
      [TOKEN_RETURN]        = {NULL,        NULL,   PREC_NONE},
      [TOKEN_SUPER]         = {NULL,        NULL,   PREC_NONE},
      [TOKEN_THIS]          = {NULL,        NULL,   PREC_NONE},
      [TOKEN_TRUE]          = {literal,        NULL,   PREC_NONE},
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
        error("Expect expression (prefix).");
        return;
    }

    COMPILER_DEBUG_LOG("parse_precedence\n");

    bool can_assign = prec <= PREC_ASSIGMENT;
    prefix(can_assign);
    //printf("PREFIX='%.*s'\n", parser.previous.length, parser.previous.start);

    while(prec <= get_rule(parser.current.type)->prec) {
        COMPILER_DEBUG_LOG("parse_precedence.infix\n");
        advance();
        ParseFunc infix = get_rule(parser.previous.type)->infix;
        infix(can_assign);
        //printf("INFIX='%.*s'\n", parser.previous.length, parser.previous.start);
    }

    if (can_assign && match_token(TOKEN_EQ)) {
        error("Invalid assigment target.");
    }
};

void literal(bool canAssign) {
    switch (parser.previous.type)
    {
    case TOKEN_TRUE: emit_byte(OP_TRUE); break;
    case TOKEN_FALSE: emit_byte(OP_FALSE); break;
    case TOKEN_NULL: emit_byte(OP_NULL); break;

    default:
        return;
    }
}

void unary(bool canAssign) {
    // -a

    TOKEN_TYPE operator = parser.previous.type;
    
    // compile operand
    parse_precedence(PREC_UNARY);

    switch (operator)
    {
        case TOKEN_MINUS: emit_byte(OP_NEGATE); break;
        case TOKEN_BANG: emit_byte(OP_NOT); break;
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

    // comparison
    case TOKEN_BANG_EQUAL: emit_bytes(OP_EQUAL, OP_NOT); break;
    case TOKEN_LESS_EQ: emit_bytes(OP_GREATER, OP_NOT); break;
    case TOKEN_GT_EQ: emit_bytes(OP_LESS, OP_NOT); break;
    case TOKEN_LESS: emit_byte(OP_LESS); break;
    case TOKEN_GT: emit_byte(OP_GREATER); break;
    case TOKEN_EQ_EQ: emit_byte(OP_EQUAL); break;

    default:
        return;
    }
};

void grouping(bool canAssign) {
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

void print_statement() {
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
    emit_byte(OP_PRINT);
};

void expression_statement() {
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after expression (expression_statement).");
    emit_byte(OP_POP);
};

void block() {
    while(!check(TOKEN_RIGHT_BRACE) && !check(TOKEN_EOF)) {
        declaration();
    }

    consume(TOKEN_RIGHT_BRACE, "Expect ';' after block.");
};

void statement() {
    if (match_token(TOKEN_PRINT)) {
        print_statement();
    } 
    else if (match_token(TOKEN_LEFT_BRACE)) {
        scope_begin();
        block();
        scope_end();
    }
    else {
        expression_statement();
    }
};

// sync compiler parser state when getting some error.
void compiler_sync() {
    parser.panic_mode = false;
    while(parser.current.type != TOKEN_EOF) {
        if (parser.previous.type == TOKEN_SEMICOLON) return;
        switch (parser.current.type)
        {
        case TOKEN_CLASS:
        case TOKEN_FUN:
        case TOKEN_VAR:
        case TOKEN_FOR:
        case TOKEN_IF:
        case TOKEN_WHILE:
        case TOKEN_PRINT:
        case TOKEN_RETURN:
            return;
        
        default:
        }

        advance();
    }
};

void named_variable(Token token, bool canAssign) {

    uint8_t getOp, setOp;
    int arg = resolve_local(current_comp, &token);
    if (arg != -1) {
        // its local var
        getOp = OP_GET_LOCAL;
        setOp = OP_SET_LOCAL;
    }
    else {
        // its global var
        arg = make_id_constant(&token);
        getOp = OP_GET_GLOBAL;
        setOp = OP_SET_GLOBAL;
    }

    if (canAssign && match_token(TOKEN_EQ)) {
        expression(); // parse arg
        emit_bytes(setOp, (uint8_t)arg);
    }
    else {
        emit_bytes(getOp, (uint8_t)arg);
    }
} 

void variable(bool canAssign) {
    named_variable(parser.previous, canAssign);
}

void define_variable(uint8_t global) {

    // pass code for local vars
    if (current_comp->scope_depth > 0) {
        return;
    }

    emit_bytes(OP_DEFINE_GLOBAL, global);
}

void var_decl() {
    uint8_t global = parse_variable("Expect a variable name.");
    if (match_token(TOKEN_EQ)) {
        // var foo = ...;
        COMPILER_DEBUG_LOG("var_decl.expr()\n");
        expression();
    } else {
        // var foo;
        COMPILER_DEBUG_LOG("var_decl.null\n");
        emit_byte(OP_NULL); // default value for variable
    }

    consume(TOKEN_SEMICOLON, "Expect ';' after var_decl.");
    define_variable(global);
};

void declaration() {

    COMPILER_DEBUG_LOG("declaration\n");
    
    if (match_token(TOKEN_VAR)) {
        var_decl();
    }
    else {
        statement();
    }

    if (parser.panic_mode) compiler_sync();
};

bool compile(const char* source, Chunk* chunk) {
    scanner_init(source);

    Compiler comp;
    compiler_init(&comp);

    compiling_chunk = chunk;
    parser.had_error = false;
    parser.panic_mode = false;
    
    hashtable_init(&string_constants);

    advance();
    
    while(!match_token(TOKEN_EOF)) {
        declaration();
    }

    COMPILER_DEBUG_LOG("compile ok\n");
    end_compiler();

    destroy_hashtable(&string_constants);

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

void compiler_init(Compiler* comp) {
    COMPILER_DEBUG_LOG("compiler_init\n");
    comp->local_count = 0;
    comp->scope_depth = 0;
    current_comp = comp;
};

