#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include "lexer.hpp"

class Parser {
    Lexer lexer;
    Token current;
public:
    Parser();
    void read_next_token();
};

#endif
