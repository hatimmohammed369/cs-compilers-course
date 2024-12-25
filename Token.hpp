#ifndef TOKEN_H_INCLUDED
#define TOKEN_H_INCLUDED

#include <string>

enum TokenType {
    END_OF_FILE = 0,
    NUMBER = 1,
    LEFT_CURLY_BRACE = 2,
    RIGHT_CURLY_BRACE = 3,
};

class Token {
public:
    TokenType ttype;
    std::string value;
    bool is_end_marker() const {
        return this->ttype != END_OF_FILE;
    }
};

#endif
