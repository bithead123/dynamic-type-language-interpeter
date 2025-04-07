#pragma once

#include <string>
#include "token.h"
#include <vector>

class Scanner {
    private:
    int _pos;
    int _sourceLen;
    public:
    std::vector<Token> get_tokens(std::string& source);
};
