#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED

#include "token.hpp"

class Lexer {
    std::string source;
    std::string::const_iterator current;
    size_t col = 0;

    inline void skip_whitespaces() {
        while (!is_at_end() && std::isspace(*current)) {
            if (*current == '\n') {
                // Add a new line
                lines.push_back(std::string{});
                col = 0;
            } else {
                col++;
            }
            current++;
        }
        if (is_at_end()) {
            std::string::size_type last_line_break_in_input =
                source.rfind('\n');
            lines.back().assign(
                source.substr(last_line_break_in_input)
            );
        } else if (lines.back().empty()) {
            std::string::size_type cur_pos =
                std::distance(source.cbegin(), current);
            std::string::size_type begin =
                source.find_last_of('\n', cur_pos);
            if (begin == std::string::npos)
                begin = 0;
            else
                begin++;
            std::string::size_type end =
                source.find_first_of('\n', cur_pos);
            if (end == std::string::npos)
                end = source.length();
            std::string::size_type length = end - begin;
            lines.back().assign(
                source.substr(begin, length)
            );
        }
    }

    Token generate_number_token();
    Token generate_string_token();
    Token generate_identifier_token();
    Token generate_invalid_token();

public:
    std::vector<std::string> lines;

    void init(char* in, const size_t& source_len);
    Token generate_next_token();

    inline bool is_at_end() {
        return current == source.end();
    }

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
