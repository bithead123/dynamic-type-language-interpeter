#include <string>
#include "scanner.h"
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
            return _current >= _capacity;
        }

        public:
        ScanBuff(string& source) : _current(0), _capacity(source.size()), _buff(source) {  
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
};

/*
    literals
    

*/

std::vector<Token> Scanner::get_tokens(std::string& src) {

    stringstream ss;
    // source
    // next()
    // back()
}