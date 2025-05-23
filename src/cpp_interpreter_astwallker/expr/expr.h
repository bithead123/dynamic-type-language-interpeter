#pragma once

#include <boost/variant.hpp>
#include <string>
#include "../scan/token.h"
#include <boost/optional.hpp>
#include <boost/any.hpp>

struct VoidType{};
struct NoneType{};
struct BreakLoop{};
typedef boost::variant<NoneType, bool, double,  std::string, VoidType, BreakLoop> ReturnObject;

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
class Statement;
class VarDecl;
class Block;
class IfBlock;
class Logical;
class Function;

template<typename T>
class IVisitor {
    public:
    virtual T visit_binary(Binary* bin) = 0;
    virtual T visit_grouping(Grouping* group) = 0;
    virtual T visit_unary(Unary* unary) = 0;
    virtual T visit_literal(Literal* lit) = 0;
    virtual T visit_id(Identifier* lit) = 0;
    //virtual T visit_call(FunctionCall* lit) = 0;
    virtual T visit_conditional(Conditional* lit) = 0;
    virtual T visit_statement(Statement* statement) = 0;
    virtual T visit_logical(Logical* log) = 0;
    virtual T visit_func(Function* f) = 0;
    //virtual T visit_varDecl(VarDecl* t) = 0;
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

class Logical : public Expr {
    public:
    Expr* lhs;
    Expr* rhs;
    Token* oper;
    Logical(Expr *l , Expr* r, Token* op) : lhs(l), rhs(r), oper(op) {};

    std::string accept(IVisitor<std::string>& v) {
        return v.visit_logical(this);
    };

    ReturnObject inerpret(IVisitor<ReturnObject>& v)  {
        return v.visit_logical(this);
    };
};

class VarDecl {
    public:
    Token* name;
    Expr* initializer;
    VarDecl(Token* t, Expr* init) : name(t), initializer(init) {};
};

class VarDefine {
    public:
    Token* name;
    VarDefine(Token* t) : name(t) {};
};

class VarAssign {
    public:
    Token* name;
    Expr* val;
    VarAssign(Token* t, Expr* init) : name(t), val(init) {};
};

class Block {
    public:
    std::vector<Statement*> statements;
    Block(std::vector<Statement*>& s) : statements(s) {};
};

class IfBlock {
    public: 
    Expr* cond;
    Statement* then;
    Statement* els;
    IfBlock(Expr* c, Statement* th, Statement* els) : els(els), cond(c), then(th) {}; 
};

class WhileStatement {
    public: 
    Expr* cond;
    Statement* then;
    WhileStatement(Expr* c, Statement* th) :  cond(c), then(th) {}; 
};

class Function  : public Expr {
    public:
    Expr* callee;
    Token* paren;
    std::vector<Expr*> args;
    Function(Expr* _callee, Token* _paren, std::vector<Expr*>& _args) : callee(_callee), paren(_paren), args(_args) {};

    std::string accept(IVisitor<std::string>& v) {
        return v.visit_func(this);
    };

    ReturnObject inerpret(IVisitor<ReturnObject>& v)  {
        return v.visit_func(this);
    };
};

class Statement : public Expr {
    public:
        Expr* expression;
        Expr* print;
        VarDecl* varDecl;
        VarDefine* varDefine;
        VarAssign* varAssign;
        Block* block;
        IfBlock* _if;
        WhileStatement* _while;

        Statement() : _while(NULL), _if(NULL), expression(NULL), print(NULL), varDecl(NULL), varDefine(NULL), block(NULL) {};

        std::string accept(IVisitor<std::string>& v) {
            return v.visit_statement(this);
        };

        ReturnObject inerpret(IVisitor<ReturnObject>& v)  {
            return v.visit_statement(this);
        };
};

