#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED

#include "token.hpp"

class Lexer {
    char* source;
    size_t source_length;
    char* current;
    size_t line = 1;
    char* current_line;
    bool computed_current_line_length = false;
    size_t current_line_length = 1;
    Token generate_number_token();
    inline void skip_whitespaces();
    Token generate_string_token();
    Token generate_identifier_token();
    Token generate_invalid_token();
public:
    void init(char* in, const size_t& source_len);
    void reset();
    Token generate_next_token();
    bool has_next();

    static size_t errors;
    static void report_lexing_error(const std::string& error_msg) {
        errors += 1;
        std::cerr << error_msg << '\n';
    }
};

#endif
