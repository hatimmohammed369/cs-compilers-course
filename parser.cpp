#include "parser.hpp"

void Parser::init(char* in, size_t source_len) {
    lexer.init(in, source_len);
    read_next_token();
}

void Parser::read_next_token() {
    current = lexer.get_next_token();
}

bool Parser::check(const std::initializer_list<TokenType>& types ) const noexcept {
    for (TokenType t : types)
        if (current.ttype == t) return true;
    return false;
}

ParseResult Parser::parse_source() {
    return parse_expression();
}

ParseResult Parser::parse_expression() {
    std::string error;
    TreeBase* parsed_hunk;
    if (check({INTEGER})) {
        IntegerNumber* num = new IntegerNumber{current};
        read_next_token();
        parsed_hunk = reinterpret_cast<TreeBase*>(num);
    }
    return ParseResult{error, parsed_hunk};
}
