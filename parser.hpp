#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include "lexer.hpp"
#include "result.hpp"
#include "syntax_tree.hpp"

class ErrorPair {
public:
    std::string msg;
    std::string diagnostics;
};

using ParseResult = PointerValueResult<
    TreeBase*/*value type*/,
    ErrorPair/*error type*/
>;

class Parser {
    Lexer lexer;
    Token current;
    Token last_used;
    size_t _errors = 0;
public:
    void report_error(const ErrorPair& error_pair) const noexcept;
    void init(char* in, size_t source_len) noexcept;

    inline size_t errors() const noexcept {
        return _errors;
    }

    inline void read_next_token() noexcept {
        current = lexer.generate_next_token();
    }

    inline bool is_at_end() const noexcept {
        return current.ttype == TokenType::END_OF_FILE;
    }

    Token consume() noexcept;
    bool check(const std::initializer_list<TokenType>& types) const noexcept;
    void synchronize() noexcept ;
    ParseResult parse_source();
    ParseResult parse_statement();
    ParseResult parse_print();
    ParseResult parse_variable_declaration();
    ParseResult parse_expression();
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
    ParseResult parse_block();
    ParseResult parse_return();
    ParseResult parse_group();
    ParseResult parse_cast();
};

#endif
