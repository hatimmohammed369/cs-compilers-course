#include "parser.hpp"
#include "syntax_tree.hpp"

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
        case INTEGER: {
            IntegerNumber* num = new IntegerNumber{current};
            read_next_token();
            parsed_hunk = reinterpret_cast<TreeBase*>(num);
            break;
        }
        case FLOAT: {
            FloatNumber* num = new FloatNumber{current};
            read_next_token();
            parsed_hunk = reinterpret_cast<TreeBase*>(num);
            break;
        }
        case VOID: {
            Void* void_object = Void::get_instance();
            read_next_token();
            parsed_hunk = reinterpret_cast<TreeBase*>(void_object);
            break;
        }
        case TRUE: {
            Boolean* bool_object = new Boolean{true};
            read_next_token();
            parsed_hunk = reinterpret_cast<TreeBase*>(bool_object);
            break;
        }
        case FALSE: {
            Boolean* bool_object = new Boolean{false};
            read_next_token();
            parsed_hunk = reinterpret_cast<TreeBase*>(bool_object);
            break;
        }
        case STRING: {
            String* string_object = new String{current.value};
            read_next_token();
            parsed_hunk = reinterpret_cast<TreeBase*>(string_object);
            break;
        }
        default: {
            std::cerr << "Parsing " << current.ttype << " is not implemented yet\n" ;
            exit(1);
        }
    }

    return ParseResult{error, parsed_hunk};
}
