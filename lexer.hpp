#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED

#include "token.hpp"

class Lexer {
    std::string source;
    std::string::const_iterator current;
    size_t col = 0;

    Token generate_number_token();
    inline void skip_whitespaces();
    Token generate_string_token();
    Token generate_identifier_token();
    Token generate_invalid_token();
public:
    std::vector<std::string> lines;
    void init(char* in, const size_t& source_len);
    Token generate_next_token();
    inline bool is_at_end();

    size_t errors = 0;
    void report_lexing_error(
        const std::string& error_msg,
        const std::string& post_msg
    ) {
        errors += 1;
        std::cerr << "Error in line " << lines.size() << ":\n" ;
        std::cerr << error_msg << '\n';
        std::cerr << lines.back() << '\n' ;
        std::cerr << post_msg << '\n' ;
    }
};

#endif
