#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED

#include "token.hpp"

class Lexer {
    std::string source;
    std::string::const_iterator current;
    size_t line;
    std::string current_line;
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
