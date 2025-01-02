#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED

#include "token.hpp"

class Lexer {
    char* source;
    char* current;
    size_t source_length;
    Token generate_number_token();
    inline void skip_whitespaces();
    Token generate_string_token();
    Token generate_identifier_token();
public:
    void init(char* in, size_t source_len);
    void reset();
    Token get_next_token();
    bool has_next();
};

#endif
