#include <string>
#include "scanner.h"
#include "../lang.h"
#include <sstream>

/*
    int: 123 .123 123.123
*/


using namespace std;

class ScanBuff {
    private:
        size_t _current;
        size_t _capacity;
        string _buff;
        
        bool can_read() {
            return _current < _capacity;
        }

        public:
        ScanBuff(string& source) : _current(0), _capacity(source.size()), _buff(source) {  
        };

        size_t get_pos() {
            return _current;
        };

        char next() {
            if (!can_read()) {
                return EOF;
            }
            else {
                return _buff[_current++];
            }
        };

        void back(size_t sz) {
            _current -= sz;
            if (_current < 0) {
                _current = 0;
            } 
        };

        void set(size_t pos) {
            _current = pos;
        };
};

bool _is_alpha(char t) {
    return ('a' <= t && t <= 'z') || ('A' <= t && t <= 'Z');
};

bool _is_num(char t) {
    return ('0' <= t && t <= '9');
};


bool _literal_int( string& s, ScanBuff& bf) {
    
    size_t old = bf.get_pos();
    size_t sz = 0;

    bool dot_ex = false;
    for (char cur = bf.next(); cur != EOF; cur = bf.next()) {
        if (cur == '.' && !dot_ex) {
            s.append(1, cur);
            dot_ex = true;
        }
        else if (isalnum(cur)) {
            s.append(1, cur);
            sz++;
        }
        else {
            break;
        }
    }

    if (sz <= 0) {
        bf.set(old); 
        return false;
    }

    return true;
};

bool _literal_string(string& s, ScanBuff& bf) {
    size_t old = bf.get_pos();
    size_t sz = 0;

    char cur = bf.next();
    if (cur != '"') {
        bf.set(old);
        return false;
    }

    bool end_ex = false;

    do{
        cur = bf.next();

        if (cur == '"') {
            end_ex = true;
            break;
        }
        else {
            s.append(1, cur);
            sz++;
        }

    } while(cur != EOF);

    if (end_ex) {
        return true;
    }
    else {
        bf.set(old);
        return false;
    }
}

bool _identifier(string& s, ScanBuff& bf) {
    
    // alnum + _ + alpha

    size_t old = bf.get_pos();
    size_t sz = 0;

    auto _is_id_symbol = [](char t) {
        return _is_alpha(t) || _is_num(t) || (t == '_');
    } ;

    char cur = bf.next();
    while(cur != EOF && _is_id_symbol(cur) == true) {
        s.append(1, cur);
        cur = bf.next();
        sz++;
    }

    if (sz > 0) {
        return true;
    }
    else return false;
}

/*
    literals


*/

std::vector<Token> Scanner::get_tokens(std::string& src) {

    vector<Token> ls;

    ScanBuff sb(src);
    string current;

    if (_identifier(current, sb)) {
        Lang::Log(WARNING, "%s\n", current.c_str());
    }



    return ls;
};