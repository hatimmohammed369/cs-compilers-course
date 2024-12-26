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
    Parser();
    void read_next_token();
};

#endif
