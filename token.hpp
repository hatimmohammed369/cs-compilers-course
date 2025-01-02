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
    TOKEN_KEYWORD_VOID = 5,
    TOKEN_KEYWORD_TRUE = 6,
    TOKEN_KEYWORD_FALSE = 7,
    TOKEN_STRING = 8,
    TOKEN_LEFT_ROUND_BRACE = 9,
    TOKEN_RIGHT_ROUND_BRACE = 10,
    TOKEN_BANG = 11,
    TOKEN_LOGICAL_EQUAL = 12, // ==
    TOKEN_LOGICAL_NOT_EQUAL = 13, // !=
    TOKEN_KEYWORD_AND = 14, // and
    TOKEN_KEYWORD_OR = 15, // or
    TOKEN_KEYWORD_XOR = 16, // xor
    TOKEN_MINUS = 17,
    TOKEN_EXPONENT = 18,
    TOKEN_STAR = 19,
    TOKEN_SLASH = 20,
    TOKEN_DOUBLE_SLASH = 21,
    TOKEN_PERCENT = 22,
    TOKEN_PLUS = 23,
    TOKEN_GREATER = 24,
    TOKEN_GREATER_EQUAL = 25,
    TOKEN_LESS = 26,
    TOKEN_LESS_EQUAL = 27,
    TOKEN_TILDE = 28,
    TOKEN_RIGHT_SHIFT = 29,
    TOKEN_LEFT_SHIFT = 30,
    TOKEN_BITWISE_OR = 31, // |
    TOKEN_BITWISE_XOR = 32, // ^
    TOKEN_BITWISE_AND = 33, // ^
    TOKEN_IDENTIFIER = 34,
    TOKEN_KEYWORD_INT = 35,
    TOKEN_KEYWORD_FLOAT = 36,
    TOKEN_KEYWORD_BOOL = 37,
    TOKEN_KEYWORD_STRING = 38,
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
            return "KEYWORD_TRUE" ;
        case 7:
            return "KEYWORD_FALSE" ;
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
            return "KEYWORD_AND";
        case 15:
            return "KEYWORD_OR";
        case 16:
            return "KEYWORD_XOR";
        case 17:
            return "MINUS";
        case 18:
            return "EXPONENT";
        case 19:
            return "STAR";
        case 20:
            return "SLASH";
        case 21:
            return "DOUBLE_SLASH";
        case 22:
            return "PERCENT";
        case 23:
            return "PLUS";
        case 24:
            return "GREATER";
        case 25:
            return "GREATER_EQUAL";
        case 26:
            return "LESS";
        case 27:
            return "LESS_EQUAL";
        case 28:
            return "TILDE";
        case 29:
            return "RIGHT_SHIFT";
        case 30:
            return "LEFT_SHIT";
        case 31:
            return "BITWISE_OR";
        case 32:
            return "BITWISE_XOR";
        case 33:
            return "BITWISE_AND";
        case 34:
            return "IDENTIFIER";
        case 35:
            return "KEYWORD_INT";
        case 36:
            return "KEYWORD_FLOAT";
        case 37:
            return "KEYWORD_BOOL";
        case 38:
            return "KEYWORD_STRING";
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
