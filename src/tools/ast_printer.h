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
    std::string print(Expr* expr) {
        return expr->accept(*this);
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
};