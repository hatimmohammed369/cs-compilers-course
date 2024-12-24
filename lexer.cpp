#include "lexer.hpp"

Lexer::Lexer(char* in) {
    this->current = this->source = in;
}
