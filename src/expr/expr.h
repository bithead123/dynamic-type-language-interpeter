#pragma once

#include <string>
#include "../scan/token.h"
#include <boost/optional.hpp>
#include "../tools/dump.h"

class Expr {
    public:
        virtual std::string as_string() = 0;
};


class Grouping : public Expr {
    public:
        Expr* expr;
        Grouping(Expr* exp) : expr(exp) {};
        
        std::string as_string() override {
            return expr->as_string();
        };
};

class Binary : public Expr {
    public:
        Expr* left;
        Expr* right;
        Token* oper;
        Binary(Expr* l, Expr* r, Token* op) : left(l), right(r), oper(op) {};
        
        std::string as_string() override {
            return tools::dump::parenthesize(oper->get_lex(), {left, right});
        };
    };

class Unary : public Expr {
    public:
        Token* oper;
        Expr* expr;
        Unary(Expr* expr, Token* operat) : expr(expr), oper(operat) {};
        
        std::string as_string() override {
            return tools::dump::parenthesize(oper->get_lex(), {expr});
        };
};

class Literal : public Expr {
    public:
        Token* token;
        Literal(Token* tk) {
            this->token = tk;
        };

        std::string as_string() override {
            if (token == nullptr) {
                return "None";
            }
            else {
                return token->get_lex();
            }
        };
};

/*
template<typename T>
class IVisitor {
    virtual T visit_binary(Binary* bin) = 0;
    virtual T visit_grouping(Grouping* group) = 0;
    virtual T visit_unary(Unary* unary) = 0;
    virtual T visit_literal(Literal* lit) = 0;
};
*/