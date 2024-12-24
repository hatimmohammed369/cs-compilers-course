#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED

#include "common.h"
#include "Token.hpp"

class Lexer {
    char* source;
    char* current;
    size_t source_length;
public:
    Lexer(char* in, size_t source_len);
    Token get_next_token();
    bool has_next();
};

#endif
