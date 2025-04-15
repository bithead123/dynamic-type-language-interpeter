#pragma once

#include <boost/variant.hpp>
#include <string>
#include "../scan/token.h"
#include <boost/optional.hpp>
#include <boost/any.hpp>
#include "../expr/expr.h"

namespace obj {
    std::string to_str(ReturnObject& t) {
        int pos = t.which();
    
        double v3;
        bool v2;
        std::string v4;
    
        switch (pos)
        {
        case 0:
            return "None";
        case 1:
            
            getVariant<bool>(t, v2);
            return std::to_string(v2);
    
        case 2:
            
            getVariant<double>(t, v3);
            return std::to_string(v3);
    
        case 3:
            getVariant<std::string>(t, v4);
            return v4;
    
        case 4:
            return "<void>";

        default:
            return "<emptyValue>";
        }
    };
}