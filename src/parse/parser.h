#pragma once

#include <vector>
#include "../expr/expr.h"
#include "../scan/token.h"
#include <initializer_list>

class Parser {
    private:
    std::vector<Token*> _tokens;
    size_t _currentPos;
    size_t _endPos;
    

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
    };


    Expr* expression() {
        return equality();
    };

    Expr* equality() {
        Expr* left = comparison();

        while(match({TokenType::EQ, TokenType::NOT_EQ})) {
            Token* op = current();
            move_next();
            Expr* right = comparison();
            return new Binary(left, right, op);
        }

        return left;
    };

    Expr* comparison() {
        Expr* left = term();

        while(match({TokenType::LESS, TokenType::LESS_EQ, TokenType::GREATER, TokenType::GREATER_EQ})) {
            Token* op = current();
            move_next();
            Expr* right = term();
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
            return new Binary(left, right, op);
        }

        return left;
    };

    Expr* unary() {
        if (match({TokenType::EXCL, TokenType::MINUS})) {
            Token* op = current();
            move_next();
            Expr* right = unary();
            return new Unary(right, op);
        }

        return primary();
    };

    Expr* primary() {
        auto curr = current();
        Expr* ex = NULL;

        if (match({TokenType::BOOL_FALSE})) ex = new Literal(curr);
        else if (match({TokenType::BOOL_TRUE})) ex = new Literal(curr);
        else if (match({TokenType::NONE})) return new Literal(curr);

        if (match({TokenType::LITERAL_STRING, TokenType::LITERAL_INT})) ex = new Literal(curr);
        
        if (match({TokenType::LEFT_ROUND_BR})) {
            auto expr = NULL;//fix
            if (!match({TokenType::RIGHT_ROUND_BR})) {
                // error
            }

            ex = new Grouping(ex);
        }

        if (ex == NULL) {
            // error
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



};

