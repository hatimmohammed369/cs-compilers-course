#ifndef TOKEN_H_INCLUDED
#define TOKEN_H_INCLUDED

#include "common.hpp"

enum TokenType {
    TOKEN_INVALID = -1,
    TOKEN_END_OF_FILE = 0,
    TOKEN_INTEGER = 1,
    TOKEN_FLOAT = 2,
    TOKEN_LEFT_CURLY_BRACE = 3,
    TOKEN_RIGHT_CURLY_BRACE = 4,
    TOKEN_VOID = 5,
    TOKEN_TRUE = 6,
    TOKEN_FALSE = 7,
    TOKEN_STRING = 8,
    TOKEN_LEFT_ROUND_BRACE = 9,
    TOKEN_RIGHT_ROUND_BRACE = 10,
    TOKEN_BANG = 11,
    TOKEN_LOGICAL_EQUAL = 12, // ==
    TOKEN_LOGICAL_NOT_EQUAL = 13, // !==
    TOKEN_LOGICAL_AND = 14, // &&
    TOKEN_LOGICAL_OR = 15, // ||
    TOKEN_LOGICAL_XOR = 16,
};

class Token {
public:
    TokenType ttype;
    std::string value;
    bool is_end_marker() const {
        return this->ttype == TOKEN_END_OF_FILE;
    }
};

#endif
