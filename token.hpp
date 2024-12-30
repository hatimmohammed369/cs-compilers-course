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
    TOKEN_MINUS = 17,
    TOKEN_EXPONENT = 18
};

class Token {
public:
    TokenType ttype;
    std::string value;
    bool is_end_marker() const {
        return this->ttype == TOKEN_END_OF_FILE;
    }
};

static const char* token_type_name(const TokenType& ttype) {
    switch (ttype) {
        case -1:
            return "INVALID" ;
        case 0:
            return "END_OF_FILE" ;
        case 1:
            return "INTEGER" ;
        case 2:
            return "FLOAT" ;
        case 3:
            return "LEFT_CURLY_BRACE" ;
        case 4:
            return "RIGHT_CURLY_BRACE" ;
        case 5:
            return "VOID" ;
        case 6:
            return "TRUE" ;
        case 7:
            return "FALSE" ;
        case 8:
            return "STRING" ;
        case 9:
            return "LEFT_ROUND_BRACE" ;
        case 10:
            return "RIGHT_ROUND_BRACE" ;
        case 11:
            return "BANG" ;
        case 12:
            return "LOGICAL_EQUAL";
        case 13:
            return "LOGICAL_NOT_EQUAL";
        case 14:
            return "LOGICAL_AND";
        case 15:
            return "LOGICAL_OR";
        case 16:
            return "LOGICAL_XOR";
        case 17:
            return "MINUS";
        case 18:
            return "EXPONENT";
    }
    return "MISSING_CATEGORY" ;
}

inline std::ostream& operator<<(std::ostream& os, const TokenType& ttype) {
    return os << std::string(token_type_name(ttype)) ;
}

inline std::ostream& operator<<(std::ostream& os, const Token& token) {
    return os << "Token{ value='" << token.value << "', ttype=" << token.ttype << " }";
}

#endif
