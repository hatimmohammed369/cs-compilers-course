#ifndef TOKEN_H_INCLUDED
#define TOKEN_H_INCLUDED

#include "common.h"

enum TokenType {
    INVALID = -1,
    END_OF_FILE = 0,
    INTEGER = 1,
    FLOAT = 2,
    LEFT_CURLY_BRACE = 3,
    RIGHT_CURLY_BRACE = 4,
    VOID = 5,
};

inline std::ostream& operator<<(std::ostream& os, const TokenType& ttype) {
    switch (ttype) {
        case -1:
            os << std::string("INVALID") ;
            break;
        case 0:
            os << std::string("END_OF_FILE") ;
            break;
        case 1:
            os << std::string("INTEGER") ;
            break;
        case 2:
            os << std::string("FLOAT") ;
            break;
        case 3:
            os << std::string("LEFT_CURLY_BRACE") ;
            break;
        case 4:
            os << std::string("RIGHT_CURLY_BRACE") ;
            break;
        case 5:
            os << std::string("VOID") ;
            break;
        default:
            os << std::string("MISSING_CATEGORY") ;
    }
    return os;
}

class Token {
public:
    TokenType ttype;
    std::string value;
    bool is_end_marker() const {
        return this->ttype == END_OF_FILE;
    }
};

inline std::ostream& operator<<(std::ostream& os, const Token& token) {
    return os << "Token{ value='" << token.value << "', ttype=" << token.ttype << " }";
}

#endif
