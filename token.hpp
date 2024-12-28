#ifndef TOKEN_H_INCLUDED
#define TOKEN_H_INCLUDED

#include "common.hpp"

enum TokenType {
    INVALID = -1,
    END_OF_FILE = 0,
    INTEGER = 1,
    FLOAT = 2,
    LEFT_CURLY_BRACE = 3,
    RIGHT_CURLY_BRACE = 4,
    VOID = 5,
    TRUE = 6,
    FALSE = 7,
    STRING = 8,
    LEFT_ROUND_BRACE = 9,
    RIGHT_ROUND_BRACE = 10,
};

class Token {
public:
    TokenType ttype;
    std::string value;
    bool is_end_marker() const {
        return this->ttype == END_OF_FILE;
    }
};

#endif
