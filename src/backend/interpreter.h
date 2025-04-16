#pragma once
#include "../expr/expr.h"
#include "../scan/token.h"
#include  <boost/any.hpp>
#include <boost/smart_ptr.hpp>
#include <vector>
#include "../backend/obj.h"

using namespace std;

class RuntimeError {
    public:
    string msg;
    int line;
    RuntimeError(string&& msg, int line): msg(msg), line(line) {};
};

class Environment {
    std::map<std::string, ReturnObject> vars;
    
    void var_set(string& name, ReturnObject& value) {
        vars[name] = value;
    };

    public:

        ReturnObject var_get(std::string& name) {
            return vars[name];
        };

        bool var_contains(std::string& name) {
            if (vars.find(name) != end(vars)) return true;
            else return false;
        };

        bool var_assign(std::string& name, ReturnObject& v, string& error_msg) {
            if (!var_contains(name)) {
                error_msg = "Variable doesn't exist in this context.";
                return false;
            }

            vars[name] = v;
            return true;
        };

        bool var_decl(std::string& name, ReturnObject& v, string& error_msg) {
            if (var_contains(name)) {
                error_msg = "Variable already defined.";
                return false;
            }

            vars[name] = v;
            return true;
        };

        bool var_define(std::string& name, string& error_msg) {
            if (var_contains(name)) {
                error_msg = "Variable already defined.";
                return false;
            }

            ReturnObject default_init = NoneType();
            var_set(name, default_init);
            return true;
        };
};

class Interpreter : IVisitor<ReturnObject> {
    private:
    bool _hadErrors;
    unique_ptr<Environment> _env;
    std::vector<unique_ptr<RuntimeError>> errors;

    ReturnObject resolve_token(Token* t) {
        switch (t->get_type())
        {
        case LITERAL_INT:
            return std::stof(t->get_lex());
        
        case LITERAL_STRING:
            return t->get_lex();
        
        case NONE:
            return NoneType();
    
        case BOOL_FALSE:
            return false;
    
        case BOOL_TRUE:
            return true;
    
        default:
            return NoneType();
        }
    } 

    bool is_math_op(TokenType t) {
        switch (t)
        {
        case MINUS:
        case PLUS:
        case STAR:
        case SLASH:
            return true;
        
        default:
            return false;
        }
    };

    bool is_logic_op(TokenType t) {
        switch (t)
        {
        case EQ_EQ:
        case NOT_EQ:
            return true;
        
        default:
            return false;
        }
    };

    template<typename CheckType>
    bool check_operand(ReturnObject t) {
        CheckType te;
        if (getVariant<CheckType>(t, te)) return true;
        else return false;
    }

    template<typename CheckType>
    bool is_type(initializer_list<ReturnObject> types) {
        if (types.size() == 0) return false;
        for (auto &tp : types) {
            CheckType t;
            if (!getVariant<CheckType>(tp, t)) return false;
        }

        return true;
    }

    void runtime_erorr(const char* msg) {
        _hadErrors = true;
        errors.push_back(make_unique<RuntimeError>(msg, 0));
    }

    ReturnObject visit_literal(Literal* t) {
        auto s = resolve_token(t->token);
        return s;
    };

    ReturnObject visit_grouping(Grouping* t) {
        return evaluate(t->expr);
    }

    ReturnObject evaluate(Expr* expr) {
        return expr->inerpret(*this);
    };

    ReturnObject visit_unary(Unary* t) {
        ReturnObject right = evaluate(t->expr);
        switch (t->oper->get_type())
        {
        case TokenType::MINUS:
            // resolve type cast
            if (!check_operand<double>(right))
            {
                runtime_erorr("<Unary> right op can be type of Double");
                return NoneType();
            }

            double _double;
            getVariant<double>(right, _double);
            return -_double;

        case TokenType::EXCL: // !
            if (!check_operand<bool>(right))
            {
                runtime_erorr("<Unary> right op can be type of Bool");
                return NoneType();
            }
            
            bool _bool;
            getVariant<bool>(right, _bool);
            return !_bool;

        default:
            if (!check_operand<bool>(right))
            {
                runtime_erorr("<Unary> Can't resolve type.");
                return NoneType();
            }
            break;
        }

        return NoneType();
    };

    ReturnObject visit_binary(Binary* t) {
        ReturnObject l = evaluate(t->left);
        ReturnObject r = evaluate(t->right);
        
        // resolve types
        if (is_math_op(t->oper->get_type()))
        {
            if (!check_operand<double>(l) || !check_operand<double>(r))
            {
                runtime_erorr("<Binary> left and right operands should have Double type.");
                return NoneType();
            }

            double lr;
            double rt;
            getVariant<double>(l, lr);
            getVariant<double>(r, rt);

            switch (t->oper->get_type())
            {
            case MINUS:
                return lr-rt;
    
            case PLUS:
                return lr+rt;
    
            case SLASH:
                return lr/rt;
            
            case STAR:
                return lr*rt;
    
            default:
                runtime_erorr("<Binary> Cant cast op.");
                return NoneType();
            }
        }
        if (is_logic_op(t->oper->get_type())) {
            
            // strings
            // doubles
            // bools

            if (is_type<double>({l, r})) {
                double left;
                double right;
                getVariant<double>(l, left);
                getVariant<double>(r, right);

                switch (t->oper->get_type())
                {
                case EQ_EQ:
                    return bool(left == right);
                
                case NOT_EQ:
                    return bool(left != right);

                default:
                    if (!check_operand<bool>(right))
                        runtime_erorr("<Binary.logic.double> Can't resolve type.");
                    return NoneType();
                }
            }
            else if (is_type<std::string>({l, r})) {
                std::string left;
                std::string right;
                getVariant<std::string>(l, left);
                getVariant<std::string>(r, right);

                switch (t->oper->get_type())
                {
                case EQ_EQ:
                    return bool(left == right);
                
                case NOT_EQ:
                    return bool(left != right);

                default:
                    if (!check_operand<bool>(right))
                        runtime_erorr("<Binary.logic.string> Can't resolve type.");
                    return NoneType();
                }
            }
            else if (is_type<bool>({l, r})) {
                bool left = boost::get<bool>(l);
                bool right = boost::get<bool>(r);

                switch (t->oper->get_type())
                {
                case EQ_EQ:
                    return bool(left == right);
                
                case NOT_EQ:
                    return bool(left != right);

                default:
                    if (!check_operand<bool>(right))
                        runtime_erorr("<Binary.logic.bool> Can't resolve type.");
                    return NoneType();
                }
            }
            
            runtime_erorr("<Binary.logic.bool> Can't resolve logic op");
                return NoneType();        
        }

        runtime_erorr("<Binary> Can't resolve Binary type.");
        return NoneType();  
    }

    ReturnObject visit_id(Identifier* t) {
        
        string varname = t->token->get_lex();
        ReturnObject val = _env.get()->var_get(varname);
        
        return val;
    }

    ReturnObject visit_call(FunctionCall* t) {
        return NoneType();
    }

    ReturnObject visit_conditional(Conditional* t) {
        return NoneType();
    }

    ReturnObject visit_statement(Statement* t) {
        if (t->expression) return t->expression->inerpret(*this);
        else if (t->varDecl) {
            string err;
            string name = t->varDecl->name->get_lex();
            ReturnObject retVal = t->varDecl->initializer->inerpret(*this);
            if (!_env.get()->var_decl(name, retVal, err)) {
                runtime_erorr(err.c_str());
                return NoneType();
            }

            return VoidType();
        }
        else if (t->varDefine) {
            string err;
            string name = t->varDefine->name->get_lex();
            if (!_env.get()->var_define(name, err)) {
                runtime_erorr(err.c_str());
                return NoneType();
            }

            return VoidType();
        }
        else {
            ReturnObject v = t->print->inerpret(*this);
            if (check_operand<double>(v)) {
                double _v;
                getVariant<double>(v, _v);
                printf("Print '%f'\n", _v);
            }
            else if (check_operand<bool>(v)) {
                bool _v;
                getVariant<bool>(v, _v);
                if (_v) printf("Print True\n");
                else printf("Print False\n");
            }
            else if (check_operand<std::string>(v)) {
                std::string _v;
                getVariant<std::string>(v, _v);
                printf("Print '%s'\n", _v.c_str());
            }
            else if (check_operand<NoneType>(v)) {
                printf("Print None\n");
            }
            else {
                Lang::Log(ERROR, "Can't resolve type in print statement\n"); 
                return VoidType();
            }

            return VoidType();
        }
    }

    

    public:
    Interpreter() : _hadErrors(false), _env(make_unique<Environment>()) {};

    void dump_errors() {
        printf("Dump runtime errors:\n");
        for (auto & t : errors) {
            printf("error: '%s'\n", (*t).msg.c_str());
        }
    }

    ReturnObject interpete(vector<Statement*> s) {
        if (!good()) {
            printf("Bad interprete\n");
            dump_errors();
            return VoidType();
        }

        for (auto &st : s) {
            ReturnObject ret = st->inerpret(*this);
            auto str = obj::to_str(ret);
            printf("out: '%s'\n", str.c_str());
        }

        return VoidType();
    }

    bool good() {
        return !_hadErrors;
    }
};