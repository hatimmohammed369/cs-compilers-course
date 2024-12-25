#include "parser.hpp"

Parser::Parser() {
    read_next_token();
}

void Parser::read_next_token() {
    current = lexer.get_next_token();
}
