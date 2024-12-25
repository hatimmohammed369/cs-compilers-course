#ifndef TOKEN_H_INCLUDED
#define TOKEN_H_INCLUDED

#include <string>

enum TokenType {
    END_OF_FILE = 0,
    NUMBER = 1,
    LEFT_CURLY_BRACE = 2,
    RIGHT_CURLY_BRACE = 3,
};

inline std::ostream& operator<<(std::ostream& os, const TokenType& ttype) {
    switch (ttype) {
        case -1:
            os << "INVALID" ;
            break;
        case 0:
            os << "END_OF_FILE" ;
            break;
        case 1:
            os << "NUMBER" ;
            break;
        case 2:
            os << "LEFT_CURLY_BRACE" ;
            break;
        case 3:
            os << "RIGHT_CURLY_BRACE" ;
            break;
        default:
            os << "MISSING_CATEGORY" ;
    }
    return os;
}

class Token {
public:
    TokenType ttype;
    std::string value;
    bool is_end_marker() const {
        return this->ttype != END_OF_FILE;
    }
};

#endif
