#pragma once

#include <boost/variant.hpp>
#include <string>
#include "../scan/token.h"
#include <boost/optional.hpp>
#include <boost/any.hpp>

struct NoneType{};
typedef boost::variant<NoneType, bool, double,  std::string> ReturnObject;

template<typename CheckType>
bool getVariant(const ReturnObject& v, CheckType& out) {
    if (const CheckType* ptr = boost::get<CheckType>(&v)) {
        out = *ptr;
        return true;
    }
    return false;
}

class Binary;
class Grouping;
class Unary;
class Literal;
class Identifier;
class FunctionCall;
class Conditional;

template<typename T>
class IVisitor {
    public:
    virtual T visit_binary(Binary* bin) = 0;
    virtual T visit_grouping(Grouping* group) = 0;
    virtual T visit_unary(Unary* unary) = 0;
    virtual T visit_literal(Literal* lit) = 0;
    virtual T visit_id(Identifier* lit) = 0;
    virtual T visit_call(FunctionCall* lit) = 0;
    virtual T visit_conditional(Conditional* lit) = 0;
};

class Expr {
    public:
    virtual std::string accept(IVisitor<std::string>& v) = 0;
    virtual ReturnObject inerpret(IVisitor<ReturnObject>& v) = 0;
};


class Grouping : public Expr {
    public:
        Expr* expr;
        Grouping(Expr* exp) : expr(exp) {};
        
        std::string accept(IVisitor<std::string>& v)  {
            return v.visit_grouping(this);
        };

        ReturnObject inerpret(IVisitor<ReturnObject>& v)  {
            return v.visit_grouping(this);
        };
};

class Binary : public Expr {
    public:
        Expr* left;
        Expr* right;
        Token* oper;
        Binary(Expr* l, Expr* r, Token* op) : left(l), right(r), oper(op) {};
        
        std::string accept(IVisitor<std::string>& v) {
           return v.visit_binary(this);
        };

        ReturnObject inerpret(IVisitor<ReturnObject>& v)  {
            return v.visit_binary(this);
        };
    };

class Unary : public Expr {
    public:
        Token* oper;
        Expr* expr;
        Unary(Expr* expr, Token* operat) : expr(expr), oper(operat) {};
        
        std::string accept(IVisitor<std::string>& v){
            return v.visit_unary(this);
        };

        ReturnObject inerpret(IVisitor<ReturnObject>& v)  {
            return v.visit_unary(this);
        };
};

class Literal : public Expr {
    public:
        Token* token;
        Literal(Token* tk) {
            this->token = tk;
        };

        std::string accept(IVisitor<std::string>& v) {
            return v.visit_literal(this);
        };

        ReturnObject inerpret(IVisitor<ReturnObject>& v)  {
            return v.visit_literal(this);
        };
};

class Identifier : public Expr {
    public:
        Token* token;

        Identifier(Token* tk) {
            this->token = tk;
        };

        std::string accept(IVisitor<std::string>& v) {
            return v.visit_id(this);
        };

        ReturnObject inerpret(IVisitor<ReturnObject>& v)  {
            return v.visit_id(this);
        };
};

class FunctionCall : public Expr {
    public:
        Token* token;
        std::vector<Expr*> args;

        FunctionCall(Token* tk) {
            this->token = tk;
        };

        void add_arg(Expr* t) {
            args.push_back(t);
        };

        std::string accept(IVisitor<std::string>& v) {
            return v.visit_call(this);
        };

        ReturnObject inerpret(IVisitor<ReturnObject>& v)  {
            return v.visit_call(this);
        };
};

class Conditional : public Expr {
    public:
        Expr* cond;
        Expr* els;
        Expr* then;

        Conditional(Expr* cond, Expr* els, Expr* then) : cond(cond), els(els), then(then) {
        };

        std::string accept(IVisitor<std::string>& v) {
            return v.visit_conditional(this);
        };

        ReturnObject inerpret(IVisitor<ReturnObject>& v)  {
            return v.visit_conditional(this);
        };
};