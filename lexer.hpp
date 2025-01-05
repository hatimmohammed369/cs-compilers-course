#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED

#include "token.hpp"

class Lexer {
    char* source;
    char* current;
    char* last_current;

    size_t source_length;

    size_t line = 1;
    size_t last_line = 1;

    Token generate_number_token();
    inline void skip_whitespaces();
    Token generate_string_token();
    Token generate_identifier_token();
    Token generate_invalid_token();
public:
    void init(char* in, size_t source_len);
    void reset();
    Token generate_next_token();
    bool has_next();
    void backtrack();
};

#endif
