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

    Statement* declaration() {
        Statement* s = varDecl();
        if (s== NULL) s = statement();

        if (!verify(s, "Unrecognized stmt declaration.\n")) {
            return NULL;
        }

        return s;
    };

    Statement* varDecl() {
        
        if (match({TokenType::VAR})) {
            move_next();
            if (match({TokenType::IDENTIFIER})) {
                Token* name = current();
                move_next();

                if (match({TokenType::EQ})) {
                    move_next();
                    Expr* init = expression();

                    if (!verify(init, "Expect expression when init VarDecl\n")) {
                        return NULL;
                    }

                    if (match({TokenType::SEMICOLON})) {
                        move_next();
                    
                        Statement* s = new Statement();
                        s->varDecl = new VarDecl(name, init);
                        return s;
                    }
                }
                else if (match({TokenType::SEMICOLON})) {
                    move_next();

                    Statement* s = new Statement();
                    s->varDefine = new VarDefine(name);
                    return s;
                }
            }
        }

        return NULL;
    };

    Statement* printStatement() {
        if (match({TokenType::PRINT})) {
            move_next();
            auto ex = expression();
            if (!verify(ex, "Expect expression after Print statement\n")) {
                return NULL;
            }

            if (match({TokenType::SEMICOLON})) {
                move_next();
                Statement* s= new Statement();
                s->print = ex;
                return s;
            }
        }

        return NULL;
    };

    Expr* logicOr() {
        Expr* logic = logicAnd();
        while(match({TokenType::OR})) {
            Token* oper = current();
            move_next();
            Expr* rhs = logicAnd();
            logic = new Logical(logic, rhs, oper);
        }

        return logic;
    }

    Expr* logicAnd() {
        Expr* logic = equality();
        while(match({TokenType::AND})) {
            Token* oper = current();
            move_next();
            Expr* rhs = equality();
            logic = new Logical(logic, rhs, oper);
        }

        return logic;
    }

    Statement* varAssign() {
        if (match({TokenType::IDENTIFIER})) {
            Token* id = current();
            move_next();
            if (match({TokenType::EQ})) {
                move_next();
                Expr* rhs = equality();
                if (!verify(rhs, "Expect equality after ID in <VarAssign>.")) {
                    return NULL;
                }

                if (match({TokenType::SEMICOLON})) {
                    move_next();

                    Statement* s = new Statement();
                    s->varAssign = new VarAssign(id, rhs);
                    return s;
                }
            }
        }

        return NULL;
    };

    Statement* exprStatement() {
        auto ex = expression();
        if (!verify(ex, "Expect an expression.\n")) {
            return NULL;
        }

        if (match({TokenType::SEMICOLON})) {
            move_next();
            Statement* s=  new Statement();
            s->expression = ex;
            return s;
        }

        return NULL;
    };

    Statement* block() {
        std::vector<Statement*> st;
        if (match({TokenType::LEFT_FIG_BR})) {
            move_next();

            Statement* decl = declaration();
            while(decl != NULL) {
                st.push_back(decl);
                decl = declaration();
            }

            if (!match({TokenType::RIGHT_FIG_BR})) {
                Lang::Log(ERROR, "Expect '}' after open brace in <Block>.\n");
                return NULL;
            }

            move_next();
            Statement* state = new Statement();
            state->block = new Block(st);
            return state;
        }

        return NULL;
    }

    Statement* ifStatement() {
        if (match({TokenType::IF, TokenType::LEFT_ROUND_BR})) {
            move_next();
            move_next();

            Expr* cond = expression();
            if (!verify(cond, "Expect expression after '(' to set condition in <If>.\n")) {
                return NULL;
            }

            if (!match({TokenType::RIGHT_ROUND_BR})) {
                Lang::Log(ERROR, "Expect expression after ')' to close condition in <If>.\n");
                return NULL;
            }

            move_next();

            if (!match({TokenType::LEFT_FIG_BR})) {
                Lang::Log(ERROR, "Expect expression after '{' to open then state in <If>.\n");
                return NULL;
            }

            move_next();

            Statement* then = declaration();
            if (!verify(then, "Expect then declaration in <If>\n")) {
                return NULL;
            }

            if (!match({TokenType::RIGHT_FIG_BR})) {
                Lang::Log(ERROR, "Expect expression after '}' to close then state in <If>.\n");
                return NULL;
            }

            move_next();
            Statement* s = new Statement();
            s->_if = new IfBlock(cond, then, NULL);
            
            // parsing else
            if (match({TokenType::ELSE, TokenType::LEFT_FIG_BR})) {
                move_next();
                move_next();
                Statement* els = declaration();
                if (!verify(els, "Expect decl in else block\n")) {
                    return NULL;
                }

                if (!match({TokenType::RIGHT_FIG_BR})) {
                    return NULL;
                }

                move_next();
                s->_if->els = els;
                return s;
            }
            else {
                return s;
            }
        }

        return NULL;
    }

    Statement* statement() {

        Statement* exp = varAssign();
        if (!exp) exp = exprStatement();
        if (!exp) exp = ifStatement();
        if (!exp) exp = printStatement();
        if (!exp) exp = block(); 

        if (!exp) {
            Lang::Log(ERROR, "Expect expression in statement\n");
            return NULL;
        }

        return exp;
    }

    Expr* expression() {
        auto e = conditional();
        if (!e) e = comma();
        if (!e) e = logicOr();
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

    std::vector<Statement*> parse() {
        std::vector<Statement*> v;
        while(!at_end()) {
            auto st = declaration();
            if (st) 
                v.push_back(st); 
            else {
                break;
            }
        }

        Lang::Log(LogLevel::INFO, "Parse ok\n");
        
        return v;
    };


};

