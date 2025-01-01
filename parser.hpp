#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include "lexer.hpp"
#include "syntax_tree.hpp"

class ParseResult {
public:
    std::string error;
    TreeBase* parsed_hunk;

    ParseResult():
        error{std::string()}, parsed_hunk{nullptr} {}

    ParseResult(std::string err, TreeBase* base):
        error{err}, parsed_hunk{base} {}

    static ParseResult empty_parse_result() {
        return ParseResult{std::string(), nullptr};
    }

};

class Parser {
    Lexer lexer;
    Token current;
public:
    void init(char* in, size_t source_len);
    Lexer get_lexer();
    inline void read_next_token();
    Token consume();
    bool check(const std::initializer_list<TokenType>& types) const noexcept;
    ParseResult parse_source();
    ParseResult parse_expression();
    ParseResult parse_logical_xor();
    ParseResult parse_logical_or();
    ParseResult parse_logical_and();
    ParseResult parse_bitwise_xor();
    ParseResult parse_bitwise_or();
    ParseResult parse_bitwise_and();
    ParseResult parse_equality();
    ParseResult parse_comparison();
    ParseResult parse_shift();
    ParseResult parse_term();
    ParseResult parse_factor();
    ParseResult parse_exponential();
    ParseResult parse_unary();
    ParseResult parse_primary();
    ParseResult parse_literal();
    ParseResult parse_grouped_expression();
};

#endif
