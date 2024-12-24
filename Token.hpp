#ifndef TOKEN_H_INCLUDED
#define TOKEN_H_INCLUDED

#include <string>

enum TokenType {
    NUMBER = 0,
    LEFT_CURLY_BRACE = 1,
    RIGHT_CURLY_BRACE = 2,
};

class Token {
public:
    TokenType ttype;
    std::string value;
};

#endif
