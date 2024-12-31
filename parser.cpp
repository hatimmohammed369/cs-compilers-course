#include <vector>
#include "parser.hpp"
#include "object.hpp"
#include "syntax_tree.hpp"
#include "token.hpp"

void Parser::init(char* in, size_t source_len) {
    lexer.init(in, source_len);
    read_next_token();
}

Lexer Parser::get_lexer() {
    return lexer;
}

inline void Parser::read_next_token() {
    current = lexer.get_next_token();
}

Token Parser::consume() {
    Token copy = current;
    read_next_token();
    return copy;
}

bool Parser::check(const std::initializer_list<TokenType>& types ) const noexcept {
    for (TokenType t : types)
        if (current.ttype == t) return true;
    return false;
}

ParseResult Parser::parse_source() {
    if (current.is_end_marker())
        return ParseResult{};
    return parse_expression();
}

ParseResult Parser::parse_expression() {
    return parse_factor();
}

ParseResult Parser::parse_factor() {
    ParseResult result = parse_exponential();
    if (!result.error.empty() || !result.parsed_hunk)
        return result;
    while (
        result.error.empty() &&
        check({TOKEN_STAR, TOKEN_SLASH, TOKEN_DOUBLE_SLASH, TOKEN_PERCENT})
    ) {
        Token op = consume();
        ParseResult right = parse_exponential();
        if (right.parsed_hunk) {
            result.parsed_hunk = reinterpret_cast<TreeBase*>(
                new Factor{result.parsed_hunk, op, right.parsed_hunk}
            );
        } else if (!right.error.empty()) {
            result.parsed_hunk = nullptr;
            result.error = right.error;
        } else {
            result.parsed_hunk = nullptr;
            result.error = "Expected expression after ";
            result.error.append(op.value);
        }
    }
    return result;
}

ParseResult Parser::parse_exponential() {
    ParseResult result = parse_unary();
    if (!result.parsed_hunk)
        return result;
    std::vector<TreeBase*> items;
    items.push_back(result.parsed_hunk);
    while (result.error.empty() && check({TOKEN_EXPONENT})) {
        Token op = consume();
        ParseResult exponent = parse_unary();
        if (exponent.parsed_hunk) {
            items.push_back(exponent.parsed_hunk);
        } else if (!exponent.error.empty()) {
            result.parsed_hunk = nullptr;
            result.error = exponent.error;
        } else {
            result.parsed_hunk = nullptr;
            result.error = "Expected expression after **";
        }
    }
    if (result.error.empty()) {
        while (items.size() >= 2) {
            TreeBase* exponent = items.back();
            items.pop_back();
            TreeBase* base = items.back();
            items.pop_back();
            items.push_back(
                reinterpret_cast<TreeBase*>(
                    new Exponential{base, exponent}
                )
            );
        }
        result.parsed_hunk = items.back();
        items.pop_back();
    }
    return result;
}

ParseResult Parser::parse_unary() {
    if (!check({TOKEN_BANG, TOKEN_MINUS, TOKEN_PLUS}))
        return parse_primary();
    ParseResult result;
    Token op = consume();
    result = parse_unary();
    if (!result.error.empty()) {
        return result;
    } else if (!result.parsed_hunk) {
        result.parsed_hunk = nullptr;
        result.error.append("Expected expression after ");
        result.error.append(op.value);
    } else {
        Unary* unary = new Unary{op, result.parsed_hunk};
        result.parsed_hunk =
            reinterpret_cast<TreeBase*>(unary);
    }
    return result;
}

ParseResult Parser::parse_primary() {
    return (
        check({TOKEN_LEFT_ROUND_BRACE}) ?
        parse_grouped_expression() :
        parse_literal()
    );
}

ParseResult Parser::parse_literal() {
    std::string error;
    TreeBase* parsed_hunk = nullptr;
    switch (current.ttype) {
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
        case TOKEN_INTEGER: {
            ObjectInteger* obj = new ObjectInteger{std::stoll(consume().value)};
            Literal* int_literal =
                new Literal{reinterpret_cast<Object*>(obj)};
            parsed_hunk = reinterpret_cast<TreeBase*>(int_literal);
            break;
        }
        case TOKEN_FLOAT: {
            ObjectFloat* obj = new ObjectFloat{std::stold(consume().value, nullptr)};
            Literal* float_literal =
                new Literal{reinterpret_cast<Object*>(obj)};
            parsed_hunk = reinterpret_cast<TreeBase*>(float_literal);
            break;
        }
        case TOKEN_STRING: {
            ObjectString* obj = new ObjectString{consume().value};
            Literal* string_literal =
                new Literal{reinterpret_cast<Object*>(obj)};
            parsed_hunk = reinterpret_cast<TreeBase*>(string_literal);
            break;
        }
        default: {}
    }
    return ParseResult{error, parsed_hunk};
}

ParseResult Parser::parse_grouped_expression() {
    read_next_token(); // Skip (
    ParseResult result = parse_expression();
    if (!result.error.empty()) {
        // Syntax error occurred
        return result;
    } else if (!result.parsed_hunk) {
        result.parsed_hunk = nullptr;
        result.error = "Expected expression after (";
    } else if (!check({TOKEN_RIGHT_ROUND_BRACE})) {
        result.parsed_hunk = nullptr;
        result.error = "Expected ) after expression";
    } else {
        // Skip )
        read_next_token();
        GroupedExpression* grouped_expr =
            new GroupedExpression{result.parsed_hunk};
        result.parsed_hunk =
            reinterpret_cast<TreeBase*>(grouped_expr);
    }
    return result;
}
