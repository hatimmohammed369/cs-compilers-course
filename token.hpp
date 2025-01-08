#ifndef TOKEN_H_INCLUDED
#define TOKEN_H_INCLUDED

#include "common.hpp"

enum class TokenType : int {
    INVALID = -1,
    END_OF_FILE = 0,
    INTEGER = 1,
    FLOAT = 2,
    LEFT_CURLY_BRACE = 3,
    RIGHT_CURLY_BRACE = 4,
    KEYWORD_VOID = 5,
    KEYWORD_TRUE = 6,
    KEYWORD_FALSE = 7,
    STRING = 8,
    LEFT_ROUND_BRACE = 9,
    RIGHT_ROUND_BRACE = 10,
    BANG = 11,
    LOGICAL_EQUAL = 12, // ==
    LOGICAL_NOT_EQUAL = 13, // !=
    KEYWORD_AND = 14, // and
    KEYWORD_OR = 15, // or
    KEYWORD_XOR = 16, // xor
    MINUS = 17,
    EXPONENT = 18,
    STAR = 19,
    SLASH = 20,
    DOUBLE_SLASH = 21,
    PERCENT = 22,
    PLUS = 23,
    GREATER = 24,
    GREATER_EQUAL = 25,
    LESS = 26,
    LESS_EQUAL = 27,
    TILDE = 28,
    RIGHT_SHIFT = 29,
    LEFT_SHIFT = 30,
    BITWISE_OR = 31, // |
    BITWISE_XOR = 32, // ^
    BITWISE_AND = 33, // &
    IDENTIFIER = 34,
    KEYWORD_INT = 35,
    KEYWORD_FLOAT = 36,
    KEYWORD_BOOLEAN = 37,
    KEYWORD_STRING = 38,
    COLON_EQUAL = 39,
    COMMA = 40,
    SEMI_COLON = 41,
    LINEBREAK = 42
};

class Token {
public:
    TokenType ttype;
    std::string value;
    bool is_end_marker() const {
        return this->ttype == TokenType::END_OF_FILE;
    }
};

static const char* token_type_name(const TokenType& ttype) {
    int int_ttype = static_cast<int>(ttype);
    switch (int_ttype) {
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
            return "KEYWORD_BOOLEAN";
        case 38:
            return "KEYWORD_STRING";
        case 39:
            return "COLON_EQUAL";
        case 40:
            return "COMMA";
        case 41:
            return "SEMI_COLON";
        case 42:
            return "LINEBREAK";
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
