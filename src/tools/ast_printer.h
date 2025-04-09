#pragma once

#include "../expr/expr.h"
#include <string>

class AstPrinter  {
    public:
    std::string print(Expr* expr) {
        return expr->as_string();
    };
};