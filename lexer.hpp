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
    inline bool has_next();

    static size_t errors;
    void report_lexing_error(
        const std::string& error_msg,
        const std::string& post_msg
    ) {
        errors += 1;
        std::cerr << "Error in line " << line << ":\n" ;
        std::cerr << error_msg << '\n';
        std::cerr << current_line << '\n' ;
        std::cerr << post_msg << '\n' ;
    }
};

#endif
