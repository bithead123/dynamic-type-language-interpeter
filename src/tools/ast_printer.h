#pragma once

#include "../expr/expr.h"
#include <string>

using namespace std;

class AstPrinter : IVisitor<string> {
    private:
    std::string parenthesize(std::string name, initializer_list<Expr*> exprs) {
        std::string s("(");
        s.append(name);
    
        for (auto & ex : exprs) {
            s.append(" ");
            auto str = ex->accept(*this);
            s.append(str);
        }
        
        s.append(")");
    
        return s;
    };  
    
    public:
    void print(vector<Statement*>& t) {
        for (auto &s : t) {
            auto str= s->accept(*this);
            printf("%s\n", str.c_str());
        }
    };

    string visit_binary(Binary* bin) {
        return parenthesize(bin->oper->get_lex(), {bin->left, bin->right});
    };

    string visit_unary(Unary* u) {
        if (u->oper == NULL) {
            printf("visit_unary.errpr\n");
        }
        return parenthesize(u->oper->get_lex(), {u->expr});
    };

    string visit_literal(Literal* u) {
        if (u->token == nullptr) {
            return "None";
        }
        else {
            return u->token->get_lex();
        }
    };

    string visit_grouping(Grouping* g) {
        return parenthesize("group", {g->expr});
    };

    string visit_id(Identifier* g) {
        string s(g->token->get_lex());
        return g->token->get_lex();
    };

    string visit_conditional(Conditional* g) {
        string s("(");
        s.append(g->cond->accept(*this));
        s.append(" ? ");
        s.append(g->then->accept(*this));
        s.append(" : ");
        s.append(g->els->accept(*this));
        s.append(")");

        return s;
    };

    string visit_call(FunctionCall* g) {
        string s(g->token->get_lex());
        if (g->args.size() == 0) {
            s.append("()");
        }
        else {
            s.append(1, '(');
            
            for (auto &ar : g->args) {
                auto arg_str = ar->accept(*this);
                s.append(arg_str);
                s.append(1, ',');
            }
            s.append(1, ')');
        }

        return s;
    };

    string visit_statement(Statement* s) {
        string str("(stmt ");
        if (s->expression) str.append(s->expression->accept(*this));
        else if (s->varDecl) {
            str.append("<VarDecl> "); // <VarDecl> a=
            str.append(s->varDecl->name->get_lex());
            str.append(" = ");
            str.append(s->varDecl->initializer->accept(*this));
        }
        else if (s->varDefine) {
            str.append("<VarDefine> ");
            str.append(s->varDefine->name->get_lex());
        }
        else if (s->print) {
            str.append("<Print> ");
            str.append(s->print->accept(*this));
        }
        
        str.append(")");

        return str;
    };
};