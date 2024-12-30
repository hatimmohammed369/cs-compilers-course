#include "parser.hpp"
#include "object.hpp"

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
    TreeBase* parsed_hunk = nullptr;
    if (current.is_end_marker()) {
        return ParseResult{error, parsed_hunk};
    }

    switch (current.ttype) {
        case TOKEN_INTEGER: {
            ObjectInteger* obj = new ObjectInteger{std::stoll(current.value)};
            Literal* int_literal =
                new Literal{reinterpret_cast<Object*>(obj)};
            read_next_token();
            parsed_hunk = reinterpret_cast<TreeBase*>(int_literal);
            break;
        }
        case TOKEN_FLOAT: {
            ObjectFloat* obj = new ObjectFloat{std::stold(current.value, nullptr)};
            Literal* float_literal =
                new Literal{reinterpret_cast<Object*>(obj)};
            read_next_token();
            parsed_hunk = reinterpret_cast<TreeBase*>(float_literal);
            break;
        }
        case TOKEN_VOID: {
            ObjectVoid* obj = ObjectVoid::get_void_object();
            Literal* void_literal =
                new Literal{reinterpret_cast<Object*>(obj)};
            read_next_token();
            parsed_hunk = reinterpret_cast<TreeBase*>(void_literal);
            break;
        }
        case TOKEN_TRUE: {
            ObjectBoolean* obj = ObjectBoolean::get_true_object();
            Literal* bool_literal =
                new Literal{reinterpret_cast<Object*>(obj)};
            read_next_token();
            parsed_hunk = reinterpret_cast<TreeBase*>(bool_literal);
            break;
        }
        case TOKEN_FALSE: {
            ObjectBoolean* obj = ObjectBoolean::get_false_object();
            Literal* bool_literal =
                new Literal{reinterpret_cast<Object*>(obj)};
            read_next_token();
            parsed_hunk = reinterpret_cast<TreeBase*>(bool_literal);
            break;
        }
        case TOKEN_STRING: {
            ObjectString* obj = new ObjectString{current.value};
            Literal* string_literal =
                new Literal{reinterpret_cast<Object*>(obj)};
            read_next_token();
            parsed_hunk = reinterpret_cast<TreeBase*>(string_literal);
            break;
        }
        default: {
            std::cerr << "Parsing " << current.ttype << " is not implemented yet\n" ;
            exit(1);
        }
    }

    return ParseResult{error, parsed_hunk};
}
