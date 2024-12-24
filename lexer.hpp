#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED

class Lexer {
    char* source;
    char* current;
public:
    Lexer(char* in);
};

#endif
