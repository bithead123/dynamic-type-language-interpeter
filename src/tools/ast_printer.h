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
        printf("BINARY\n");
        printf("op=%s\n", bin->oper->get_lex().c_str());
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
};