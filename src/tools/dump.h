#pragma once
#include <string>

namespace tools {
    namespace dump {
        template<typename AsStringObject>
        std::string parenthesize(std::string name, std::initializer_list<AsStringObject*> exprs) {
            std::string s("(");
            s.append(name);

            for (auto & ex : exprs) {
                s.append(" ");
                auto str = ex->as_string();
                s.append(str);
            }
            
            s.append(")");

            return s;
        };   
    }
}