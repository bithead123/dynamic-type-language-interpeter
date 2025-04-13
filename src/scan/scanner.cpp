#include "scanner.h"
#include "../tools/log.h"
#include <sstream>
#include <string>
#include <map>
 #include <boost/filesystem.hpp>

using namespace std;

map<const char*, TokenType> _keywords {
    {"and", AND},
    {"or", OR},
    {"class", CLASS},
    {"else", ELSE},
    {"if", IF},
    {"true", BOOL_TRUE},
    {"false", BOOL_FALSE},
    {"fun", FUN},
    {"for", FOR},
    {"none", NONE},
    {"print", PRINT},
    {"return", RETURN},
    {"super", SUPER},
    {"this", THIS},
    {"var", VAR},
    {"while", WHILE},
    {"eof", EOF_},
};

class ScanBuff {
private:
  size_t _current;
  size_t _capacity;
  string _buff;

public:
  ScanBuff(string &source)
      : _current(0), _capacity(source.size()), _buff(source){};

  size_t get_pos() { return _current; };

  bool can_read() { return _current < _capacity; }

  char next() {
    if (!can_read()) {
      _current++;
      return EOF;
    } else {
      return _buff[_current++];
    }
  };

  void back(size_t sz) {
    _current -= sz;
    if (_current < 0) {
      _current = 0;
    }
  };

  void skip(string to_skip) {
    char t = next();
    while(t != EOF) {
        if (to_skip.find_first_of(t, 0) != string::npos) {
            t = next();
        }
        else {
            back(1);
            return;
        }
    }
  }

  void set(size_t pos) { _current = pos; };
};

bool _is_space(char t) {
    switch (t)
    {
    case '\r':
    case '\t':
    case ' ':
        return true;
    
    default:
        return false;
    }
};

bool _is_math(char t) {
  switch (t)
  {
  case '+':
  case '-':
  case '*':
  case '/':
      return true;
  
  default:
      return false;
  }
};

bool _is_endline(char t) {
    return t == '\n';
};

bool _is_end_instr(char t) {
  return t == ';' || t == ')';
};

bool _is_delimiter(char t) {
  return t == ',';
};

bool _is_alpha(char t) {
  return ('a' <= t && t <= 'z') || ('A' <= t && t <= 'Z');
};

bool _is_num(char t) { return ('0' <= t && t <= '9'); };

bool _literal_int(string &s, ScanBuff &bf) {

  size_t old = bf.get_pos();
  size_t sz = 0;
  s.clear();

  bool dot_ex = false;
  for (char cur = bf.next(); cur != EOF; cur = bf.next()) {
    if (cur == '.' && !dot_ex) {
      s.append(1, cur);
      dot_ex = true;
    } else if (_is_num(cur)) {
      s.append(1, cur);
      sz++;
    } else if (_is_space(cur) || _is_endline(cur) || _is_math(cur) || _is_delimiter(cur) || _is_end_instr(cur) || !bf.can_read()) {
      break;
    }
    else {
        bf.set(old);
        return false;
    }
  }

  if (sz <= 0) {
    bf.set(old);
    return false;
  }

  bf.back(1);
  return true;
};

bool _literal_string(string &s, ScanBuff &bf) {
  size_t old = bf.get_pos();
  size_t sz = 0;
  s.clear();

  char cur = bf.next();
  if (cur != '"') {
    bf.set(old);
    return false;
  }

  bool end_ex = false;

  do {
    cur = bf.next();

    if (cur == '"') {
      end_ex = true;
      break;
    } else {
      s.append(1, cur);
      sz++;
    }

  } while (cur != EOF);

  if (end_ex) {
    return true;
  } else {
    bf.set(old);
    return false;
  }
}

bool _identifier(string &s, ScanBuff &bf) {

  // alnum + _ + alpha

  size_t old = bf.get_pos();
  size_t sz = 0;
  s.clear();

  auto _is_id_symbol = [](char t) {
    return _is_alpha(t) || _is_num(t) || (t == '_');
  };

  char cur = bf.next();
  while (cur != EOF && _is_id_symbol(cur) == true) {
    s.append(1, cur);
    cur = bf.next();
    sz++;
  }

  if (sz > 0) {
    bf.back(1);
    return true;
  } else
    bf.set(old);
    return false;
}

bool _single_or_two_chars_token(string& s, TokenType &type, ScanBuff &bf) {
  char cur = bf.next();
  if (cur == EOF) {
    return false;
  }

  s.clear();
  s.append(1, cur);

  char next = bf.next();

  switch (cur) {
    // bracers
    case '(':
    type = LEFT_ROUND_BR;
    bf.back(1);
    return true;

    case '{':
    type = LEFT_FIG_BR;
    bf.back(1);
    return true;

    case ')':
    type = RIGHT_ROUND_BR;
    bf.back(1);
    return true;

    case '}':
    type = RIGHT_FIG_BR;
    bf.back(1);
    return true;

    // eq
    case '<':
        if (next == '=') {
          type = LESS_EQ;
          s.append(1, next);
          return true;
        }
        type = LESS;
        bf.back(1);
        return true;

    case '>':
        if (next == '=') {
          type = GREATER_EQ;
          s.append(1, next);
          return true;
        }
        type = GREATER;
        bf.back(1);
        return true;

    case '!':
        if (next == '=') {
          type = NOT_EQ;
          s.append(1, next);
          return true;
        }
        type = EXCL;
        bf.back(1);
        return true;

    // math
  case '+':
    type = PLUS;
    bf.back(1);
    return true;
  case '-':
    type = MINUS;
    bf.back(1);
    return true;
    case '/':
    type = SLASH;
    bf.back(1);
    return true;
    case '*':
    type = STAR;
    bf.back(1);
    return true;

    case '=':
    if (next == '=') {
      type = EQ_EQ;
      s.append(1, next);
      return true;
    }
    type = EQ;
    bf.back(1);
    return true;
    
    case '.':
    type = DOT;
    bf.back(1);
    return true;

    case ';':
    type = SEMICOLON;
    bf.back(1);
    return true;

    case ':':
    type = DOT2;
    bf.back(1);
    return true;

    case ',':
    type = COMMA;
    bf.back(1);
    return true;

  default:
    type = _ERROR;
    if (next != EOF) bf.back(2); 
    else bf.back(1); 

    s.clear();
    return false;
  }
}

bool is_keyword(string& s) {
    auto fn = _keywords.find(s.c_str());
    if (fn != end(_keywords)) {
        return true;
    }
    else return false;
};

std::vector<Token*> Scanner::get_tokens(std::string &src) {

  vector<Token*> ls;
  ScanBuff sb(src);
  string current;

  int line = 0;
  while(sb.can_read()) {
    // long->short
    
    sb.skip(" \t\r");

    // literals
    if (_literal_int(current, sb)) {
        ls.push_back(new Token(current, LITERAL_INT, NULL, line));
        continue;
    }
    else if (_literal_string(current, sb)) {
      ls.push_back(new Token(current, LITERAL_STRING, NULL, line));
        continue;
    }

    if (_identifier(current, sb)) {
        if (is_keyword(current)) {
            auto type = _keywords[current.c_str()];
            ls.push_back(new Token(current,type, NULL, line));
            continue;
        } else {
            ls.push_back(new Token(current, IDENTIFIER, NULL, line));
            continue;
        } 
    }

    TokenType type;
    if (_single_or_two_chars_token(current, type, sb)) {
        ls.push_back(new Token(current, type, NULL, line));
        if (!sb.can_read()) break;
        continue;
    }

    char c = sb.next();
    if (_is_space(c)) {
        continue;
    }
    else if (_is_endline(c)) {
        line++;
        continue;
    }
    else if (c == EOF) {
        Lang::Log(WARNING, "Parsing ok.\n");
        break;
    } 
    
    // error there
    Lang::Log(ERROR, "Unexpected symbol at %i:%i, got: '%c'\n", line, sb.get_pos(), c);
    break;
  }

  return ls;
};


std::string Scanner::read_file(const char* path) {
  stringstream ss;
  string line;
  if (boost::filesystem::exists(path)) {
    boost::filesystem::ifstream fs(path);
    if (fs.good()) {
      while(getline(fs, line)) {
        ss << line;
      }
    }
  }

  return ss.str();
};