#ifndef TOKEN_H_INCLUDED
#define TOKEN_H_INCLUDED

#include <string>

enum TokenType {
    DEAD = 0,
    END_MARKER = 1,
    NUMBER = 2,
    LEFT_CURLY_BRACE = 3,
    RIGHT_CURLY_BRACE = 4,
};

class Token {
public:
    TokenType ttype;
    std::string value;
};

std::ostream& operator<<(std::ostream& os, const Token& token) {
    return os << "Token{ value='" << token.value << "', ttype=" << token.ttype << " }";
}

#endif
