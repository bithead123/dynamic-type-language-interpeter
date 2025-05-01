#include "scanner.h"

typedef struct {
    const char* start;
    const char* current;
    int line;
} Scanner;

Scanner scanner;

bool is_at_end() {
    return *scanner.current == '\0';
};

Token make_token(TOKEN_TYPE type) {
    Token t;
    t.type = type;
    t.start = scanner.start;
    t.length = (int)(scanner.current-scanner.start);
    t.line = scanner.line;
    return t;
};

Token make_eof_token() {
    Token t;
    t.type = TOKEN_EOF;
    t.start = "EOF";
    t.length = 0;
    t.line = scanner.line;
    return t;
};

Token make_error_token(const char* msg) {
    Token t;
    t.type = TOKEN_ERROR;
    t.start = msg;
    t.length = (int)strlen(msg);
    t.line = scanner.line;
    return t;
};

char next_char() {
    scanner.current++;
    return scanner.current[-1];
};

bool match(char expected) {
    if (is_at_end()) return false;
    if (*scanner.current != expected) return false;
    
    scanner.current++;
    return true;
};

char peek() {
    return *scanner.current;
};

char peek_next() {
    if (is_at_end()) return '\0';
    return scanner.current[1];
};

void skip_whitespaces() {
    for(;;) {
        char t = peek();
        switch (t)
        {
        case ' ':
        case '\r':
        case '\t':
            next_char();
            break;

        case '\n':
            scanner.line++;
            next_char();
            break;
        
        default:
            return;
        }
    }
};

bool is_digit(char t) {
    return t >= '0' && t <= '9';
};

bool is_alpha(char t) {
    return (
    (t >= 'a' && t <= 'z') ||
    (t >= 'A' && t <= 'Z') ||
    t == '_');
};

Token string() {
    while(peek() != '"' && !is_at_end()) {
        if (peek() == '\n') scanner.line++;
        next_char();
    };

    if (is_at_end()) return make_error_token("Unterminated enless string.");

    next_char();
    return make_token(TOKEN_STRING);
};

Token number() {
    while(is_digit(peek())) next_char();

    if (peek() == '.' && is_digit(peek_next())) {
        next_char();
        
        while(is_digit(peek())) next_char();
    }

    return make_token(TOKEN_NUMBER);
}

TOKEN_TYPE check_keyword(int start, int length, const char* rest, TOKEN_TYPE type) {
    if (scanner.current - scanner.start == start + length &&
        memcmp(scanner.start + start, rest, length) == 0) {
            return type;
        }

    return TOKEN_ID;
};

TOKEN_TYPE get_identifier_type() {
    
    switch (scanner.start[0])
    {
    case 'a': return check_keyword(1, 2, "nd", TOKEN_AND);
    case 'c': 
        if (scanner.current - scanner.start > 1) { // have next
            switch (scanner.start[1])
            {
            case 'l': return check_keyword(2, 3, "ass", TOKEN_CLASS);
            case 'a': return check_keyword(2, 2, "se", TOKEN_CASE);
            case 'o': return check_keyword(2, 6, "ntinue", TOKEN_CONTINUE);
            default:
                break;
            }
        }    
        break;
    case 'e': return check_keyword(1, 3, "lse", TOKEN_ELSE);
    case 'i': return check_keyword(1, 1, "f", TOKEN_IF);
    case 'n': return check_keyword(1, 3, "ull", TOKEN_NULL);
    case 'o': return check_keyword(1, 1, "r", TOKEN_OR);
    case 'p': return check_keyword(1, 4, "rint", TOKEN_PRINT);
    case 'r': return check_keyword(1, 5, "eturn", TOKEN_RETURN);
    case 's': 
        if (scanner.current - scanner.start > 1) { // have next
            switch (scanner.start[1])
            {
            case 'u': return check_keyword(2, 3, "per", TOKEN_SUPER);
            case 'w': return check_keyword(2, 4, "itch", TOKEN_SWITCH);
            default:
                break;
            }
        }    
        break;
    case 'v': return check_keyword(1, 2, "ar", TOKEN_VAR);
    case 'w': return check_keyword(1, 4, "hile", TOKEN_WHILE);
    case 'd': return check_keyword(1, 6, "efault", TOKEN_DEFAULT);
    case 'b': return check_keyword(1, 4, "reak", TOKEN_BREAK);
    case 'l': return check_keyword(1, 2, "et", TOKEN_LET);
    case 'f':
        if (scanner.current - scanner.start > 1) { // have next
            switch (scanner.start[1])
            {
            case 'a': return check_keyword(2, 3, "lse", TOKEN_FALSE);
            case 'o': return check_keyword(2, 1, "r", TOKEN_FOR);
            case 'u': return check_keyword(2, 1, "n", TOKEN_FUN);
            default:
                break;
            }
        }    
        break;

    case 't':
        if (scanner.current - scanner.start > 1) { // have next
            switch (scanner.start[1])
            {
            case 'h': return check_keyword(2, 2, "is", TOKEN_THIS);
            case 'r': return check_keyword(2, 2, "ue", TOKEN_TRUE);
            default:
                break;
            }
        }    
        break;
    
    default:
        break;
    }

    return TOKEN_ID;
};

Token identifier() {
    while(is_alpha(peek()) || is_digit(peek())) next_char();
    return make_token(get_identifier_type());
}

Token scan_token() {
    skip_whitespaces();
    if (is_at_end()) return make_eof_token();

    scanner.start = scanner.current;

    char c = next_char();

    if (is_digit(c)) return number();
    if (is_alpha(c)) return identifier();

    switch (c)
    {
    case '(': return make_token(TOKEN_LEFT_PAREN);
    case ')': return make_token(TOKEN_RIGHT_PAREN);
    case '{': return make_token(TOKEN_LEFT_BRACE);
    case '}': return make_token(TOKEN_RIGHT_BRACE);
    case ';': return make_token(TOKEN_SEMICOLON);
    case ',': return make_token(TOKEN_COMMA);
    case '.': return make_token(TOKEN_DOT);
    case '-': return make_token(TOKEN_MINUS);
    case '+': return make_token(TOKEN_PLUS);
    case ':': return make_token(TOKEN_COLON);
    case '*': return make_token(TOKEN_STAR);
    case '/': 
        if (peek_next() == '/') {
            while(peek() != '\n' && !is_at_end()) peek_next();
            break;
        }
        else return make_token(TOKEN_SLASH);

    case '!': return make_token(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '=': return make_token(match('=') ? TOKEN_EQ_EQ : TOKEN_EQ);
    case '<': return make_token(match('=') ? TOKEN_LESS_EQ : TOKEN_LESS);
    case '>': return make_token(match('=') ? TOKEN_GT_EQ : TOKEN_GT);

    // literals
    case '"': return string();

    default:
        printf("CHAR='%c'\n", c);
        return make_error_token("Unexpected character.");
    }
}

void scanner_init(const char* source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}