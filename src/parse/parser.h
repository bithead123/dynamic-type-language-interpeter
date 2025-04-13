#pragma once

#include <vector>
#include "../expr/expr.h"
#include "../scan/token.h"
#include <initializer_list>
#include "../tools/log.h"

class Parser {
    private:
    std::vector<Token*> _tokens;
    size_t _currentPos;
    size_t _endPos;
    
    void sync_to_next() {

    };

    bool verify(Token* pt, const char* msg) {
        if (pt == NULL) {
            Lang::Log(ERROR, msg);
            return false;
        }

        return true;
    };

    bool verify(Expr* ptr, const char* msg) {
        if (ptr == NULL) {
            Lang::Log(ERROR, msg);
            return false;
        }

        return true;
    };

    Token* prev() {
        if (_currentPos > 0) {
            return _tokens[_currentPos-1];
        }
        else return NULL;
    };

    Token* current() {
        if (_currentPos < _tokens.size()) {
            return _tokens[_currentPos];
        }
        else return NULL;
    };

    Token* next() {
        if (_currentPos - 1 < _tokens.size()) {
            return _tokens[_currentPos-1];
        }
        else return NULL;
    };

    void move_next() {
        _currentPos++;
    };

    void move_back() {
        if (_currentPos > 0) {
            _currentPos--;
        }
    };

    // произвольный сдвиг
    bool move(size_t to) {
        if (_currentPos + to >= 0 && _currentPos + to <= _endPos) {
            _currentPos += to;
            return true;
        }
        else return false;
    };

    bool at_end() {
        return _currentPos == _tokens.size();
    };

    // проверяет тип следующих токенов последовательно, курсор не сдвигает
    bool check(std::initializer_list<TokenType> types) {
        
        if (types.size() + _currentPos > _endPos) {
            return false;
        }

        size_t offset = _currentPos;
        for (auto  &t : types) {
            auto cur = _tokens[offset];
            if (cur && cur->get_type() == t) {
                offset++;
            }
            else {
                return false;
            }
        }
    };

    // проверяет является ли текущий токен одним из след типов
    bool match(std::initializer_list<TokenType> types) {
        
        if (at_end()) {
            return false;
        }

        Token* cur = current();
        for (auto &t : types) {
            if (cur->get_type() == t) {
                return true;
            }
        };

        return false;
    };


    Expr* expression() {
        auto e = conditional();
        if (!e) e = comma();
        return e;
    };

    Expr* conditional() {
        Expr* left = equality();

        if (match({TokenType::QUESTION})) {
            move_next();
            Expr* then = expression();
            if (!verify(then, "Expect a expression after '?' in <conditional>\n")) {
                return NULL;
            }

            if (match({TokenType::DOT2})) {
                move_next();
                Expr* els = conditional();
                if (!verify(els, "Expect a expression after ':' in <conditional>\n")) {
                    return NULL;
                }

                return new Conditional(left, els, then);
            }

            printf("Can't parse  structure <conditional>\n");
            
            return NULL;
        }

        return left;
    }

    Expr* comma() {
        Expr* left = equality();

        while(match({TokenType::COMMA})) {
            Token* op = current();
            move_next();
            Expr* right = equality();
            if (!verify(right, "Expected right expression in <comma>\n")) {
                return NULL;
            }
            return new Binary(left, right, op);
        }

        return left;
    }

    Expr* equality() {
        Expr* left = comparison();

        while(match({TokenType::EQ_EQ, TokenType::NOT_EQ})) {
            Token* op = current();
            move_next();
            Expr* right = comparison();
            if (!verify(right, "Expected right expression in <equality>\n")) {
                return NULL;
            }
            return new Binary(left, right, op);
        }

        return left;
    };

    Expr* comparison() {
        Expr* left = term();

        while(match({TokenType::LESS, TokenType::LESS_EQ, TokenType::GREATER, TokenType::GREATER_EQ})) {
            Token* op = current();
            //printf("OP=%s lex=%s\n", op->get_name(), op->get_lex().c_str());
            move_next();
            Expr* right = term();
            if (!verify(right, "Expected right expression in <comparison>\n")) {
                return NULL;
            }
            return new Binary(left, right, op);
        }

        return left;
    };

    Expr* term() {
        Expr* left = factor();
        while(match({TokenType::PLUS, TokenType::MINUS})) {
            Token* op = current();
            move_next();
            Expr* right = factor();
            if (!verify(right, "Expected right expression in <term>\n")) {
                return NULL;
            }
            return new Binary(left, right, op);
        }
        
        return left;
    };

    Expr* factor() {
        Expr* left = unary();
        
        while(match({TokenType::SLASH, TokenType::STAR})) {
            Token* op = current();
            move_next();
            Expr* right = unary();
            if (!verify(right, "Expected right expression in <factor>\n")) {
                return NULL;
            }
            return new Binary(left, right, op);
        }

        return left;
    };

    Expr* unary() {
        if (match({TokenType::EXCL, TokenType::MINUS})) {
            Token* op = current();
            verify(op, "Token is NULL\n");
            move_next();
            Expr* right = unary();
            if (!verify(right, "Expected right expression in <unary>\n")) {
                return NULL;
            }
            return new Unary(right, op);
        }

        return primary();
    };

    Expr* primary() {
        auto curr = current();
        Expr* ex = NULL;

        if (match({TokenType::BOOL_FALSE})) ex = new Literal(curr);
        else if (match({TokenType::BOOL_TRUE})) ex = new Literal(curr);
        else if (match({TokenType::NONE})) ex = new Literal(curr);
        else if (match({TokenType::LITERAL_STRING, TokenType::LITERAL_INT})) ex = new Literal(curr);
        else if (match({TokenType::IDENTIFIER})) {
            // parse fn call
            move_next();
            if (match({TokenType::LEFT_ROUND_BR})) {
                move_next();
                
                if (match({TokenType::RIGHT_ROUND_BR})) {
                    ex = new FunctionCall(curr);
                }
                else {
                    // parse args
                    FunctionCall* call = new FunctionCall(curr);
                    while(true) {
                        Expr* arg = equality();
                        if (arg != NULL) 
                            call->add_arg(arg);
    
                        if (match({TokenType::COMMA}) == true) move_next();
                        else break;
                    }
    
                    //move_next();
                    if (!match({TokenType::RIGHT_ROUND_BR})) {
                        auto cur2 = current();
                        printf("Expect close ')' after fn call, but current is '%s'\n", cur2->get_lex().c_str());
                    }
    
                    ex = call;
                }
            }
            else {
                move_back();
                ex = new Identifier(curr);
            }
        }
        // open tag
        else if (match({TokenType::LEFT_ROUND_BR})) {
            printf("LEFT BR\n");
            auto expr = expression();
            if (!match({TokenType::RIGHT_ROUND_BR})) {
                // error
            }

            ex = new Grouping(expr);
        }

        if (ex == NULL) {
            // error
            verify(ex, "Expected expression in <Primary>\n");
            return NULL;
        }

        move_next();
        return ex;
    };

    // expression
    // equality()
    // comparison()
    // term()
    // factor()
    // unary() 
    // primary()

    // check type
    public:
    Parser(std::vector<Token*>& v): _tokens(v), _currentPos(0), _endPos(v.size()) {};

    Expr* parse() {
        auto ex = expression();
        if (ex == NULL) {
            Lang::Log(LogLevel::ERROR, "can't parse expression\n");
            return NULL;
        }
        
        Lang::Log(LogLevel::INFO, "Parse ok\n");
        
        return ex;
    };


};

