#pragma once

#include "../scan/token.h"
#include "../expr/expr.h"
#include <vector>
#include <iostream>

namespace tools {
    void print_tokens(std::vector<Token*> const& tokens, std::ostream& s, char separator) {
        for (const auto& t : tokens) {
            s << t->get_name() << "(" << t->get_lex().c_str() << ")" << separator;
        }

        s << "\n";
    };
}