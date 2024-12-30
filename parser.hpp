#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include "lexer.hpp"
#include "syntax_tree.hpp"

struct ParseResult {
    std::string error;
    TreeBase* parsed_hunk;
    ParseResult(std::string err, TreeBase* base): error{err}, parsed_hunk{base} {}
};

class Parser {
    Lexer lexer;
    Token current;
public:
    void init(char* in, size_t source_len);
    void read_next_token();
    bool check(const std::initializer_list<TokenType>& types) const noexcept;
    ParseResult parse_source();
    ParseResult parse_expression();
    ParseResult parse_literal();
};

#endif
