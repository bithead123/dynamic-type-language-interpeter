#pragma once
#include <string>
#include "../expr/expr.h"

using namespace std;

class PrnVisitor : public IVisitor<string> {
    public:
        string make_prn(Expr* expr) {
            expr->accept(*this);
        } ;

        string visit_binary(Binary* b) {
            string s("");
            s.append(b->left->accept(*this));
            s.append(b->right->accept(*this));
            s.append(b->oper->get_lex());
            return s;
        };

        string visit_grouping(Grouping* t ) {
            return t->expr->accept(*this);
        };

        string visit_literal(Literal* t ) {
            return t->token->get_lex();
        };

        string visit_unary(Unary* t ) {
            string op = t->oper->get_lex();
            if (t->oper->get_type() == TokenType::MINUS) {
            // Can't use same symbol for unary and binary.
                op = "~";
            }

            return t->expr->accept(*this) + " " + op; 
        };
    };