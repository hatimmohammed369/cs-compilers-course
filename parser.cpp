#include <vector>
#include <utility>
#include "parser.hpp"
#include "object.hpp"
#include "syntax_tree.hpp"
#include "token.hpp"

void Parser::init(char* in, size_t source_len) noexcept {
    lexer.init(in, source_len);
    read_next_token();
}

Lexer Parser::get_lexer() noexcept {
    return lexer;
}

inline void Parser::read_next_token() noexcept {
    current = lexer.generate_next_token();
}

inline bool Parser::has_next() const noexcept {
    return current.ttype != TokenType::END_OF_FILE;
}

Token Parser::consume() noexcept {
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
    // Skip empty lines
    while (current.ttype == TokenType::LINEBREAK)
        read_next_token();
    Program *source_tree = new Program;
    ParseResult result;
    while (has_next()) {
        result = parse_statement();
        if (!result.error.empty()) {
            result.parsed_hunk = nullptr;
            break;
        } else if (!result.parsed_hunk) {
            break;
        }
        source_tree->statements.push_back(result.parsed_hunk);
    }
    result.parsed_hunk = (
        result.error.empty() ?
        source_tree :
        nullptr
    );
    return result;
}

ParseResult Parser::parse_statement() {
    bool use_semicolon = true;
    ParseResult result;
    if (current.is_type_keyword()) {
        result = parse_variable_declaration();
    } else {
        use_semicolon = false;
        result = parse_expression();
    }
    if (result.error.empty()) {
        if (result.error.empty() && use_semicolon && current.ttype != TokenType::SEMI_COLON) {
            result.parsed_hunk = nullptr;
            result.error = "Expected ; after statement";
        } else if (check({TokenType::SEMI_COLON, TokenType::LINEBREAK})) {
            Statement* stmt =
                reinterpret_cast<Statement*>(result.parsed_hunk);
            stmt->end_token = new Token;
            *stmt->end_token = consume();
        }
    } else {
        result.parsed_hunk = nullptr;
    }
    return result;
}

ParseResult Parser::parse_variable_declaration() {
    ParseResult result;
    Token type_token = consume();
    Type* target_type =
        Type::get_type_by_token(type_token.ttype);
    if (current.ttype != TokenType::IDENTIFIER) {
        result.parsed_hunk = nullptr;
        result.error = "Expected identifier after type";
        result.error.append(type_token.value);
        return result;
    }
    VariableDeclaration::var_value_pairs initial_values;
    ParseResult initializer;
    while (true) {
        switch (current.ttype) {
            case TokenType::IDENTIFIER: {
                initial_values.push_back(
                    std::make_pair(
                        std::move(consume().value),
                        nullptr
                    )
                );
                break;
            }
            case TokenType::COLON_EQUAL: {
                read_next_token();
                initializer = parse_expression();
                if (!initializer.error.empty()) {
                    result.parsed_hunk = nullptr;
                    result.error = initializer.error;
                    goto END;
                }
                initial_values.back().second =
                    initializer.parsed_hunk;
                break;
            }
            case TokenType::COMMA: {
                read_next_token();
                break;
            }
            case TokenType::SEMI_COLON: {
                goto END;
            }
            default: {
                result.parsed_hunk = nullptr;
                result.error = "Unexpected item";
                goto END;
            }
        }
    }
END:
    if (result.error.empty()) {
        VariableDeclaration* declarations_list =
            new VariableDeclaration {target_type, initial_values};
        result.parsed_hunk =
            reinterpret_cast<TreeBase*>(declarations_list);
    }
    return result;
}

ParseResult Parser::parse_expression() {
    ParseResult result = parse_logical_or();
    if (!result.error.empty() || !result.parsed_hunk)
        return result;
    while (
        result.error.empty() &&
        current.ttype == TokenType::KEYWORD_XOR
    ) {
        Token op = consume();
        ParseResult right = parse_logical_or();
        if (right.parsed_hunk) {
            result.parsed_hunk = reinterpret_cast<TreeBase*>(
                new Logical{result.parsed_hunk, op, right.parsed_hunk}
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

ParseResult Parser::parse_logical_or() {
    ParseResult result = parse_logical_and();
    if (!result.error.empty() || !result.parsed_hunk)
        return result;
    while (
        result.error.empty() &&
        current.ttype == TokenType::KEYWORD_OR
    ) {
        Token op = consume();
        ParseResult right = parse_logical_and();
        if (right.parsed_hunk) {
            result.parsed_hunk = reinterpret_cast<TreeBase*>(
                new Logical{result.parsed_hunk, op, right.parsed_hunk}
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

ParseResult Parser::parse_logical_and() {
    ParseResult result = parse_bitwise_xor();
    if (!result.error.empty() || !result.parsed_hunk)
        return result;
    while (
        result.error.empty() &&
        current.ttype == TokenType::KEYWORD_AND
    ) {
        Token op = consume();
        ParseResult right = parse_bitwise_xor();
        if (right.parsed_hunk) {
            result.parsed_hunk = reinterpret_cast<TreeBase*>(
                new Logical{result.parsed_hunk, op, right.parsed_hunk}
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

ParseResult Parser::parse_bitwise_xor() {
    ParseResult result = parse_bitwise_or();
    if (!result.error.empty() || !result.parsed_hunk)
        return result;
    while (
        result.error.empty() &&
        current.ttype == TokenType::BITWISE_XOR
    ) {
        Token op = consume();
        ParseResult right = parse_bitwise_or();
        if (right.parsed_hunk) {
            result.parsed_hunk = reinterpret_cast<TreeBase*>(
                new Bitwise{result.parsed_hunk, op, right.parsed_hunk}
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

ParseResult Parser::parse_bitwise_or() {
    ParseResult result = parse_bitwise_and();
    if (!result.error.empty() || !result.parsed_hunk)
        return result;
    while (
        result.error.empty() &&
        current.ttype == TokenType::BITWISE_OR
    ) {
        Token op = consume();
        ParseResult right = parse_bitwise_and();
        if (right.parsed_hunk) {
            result.parsed_hunk = reinterpret_cast<TreeBase*>(
                new Bitwise{result.parsed_hunk, op, right.parsed_hunk}
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

ParseResult Parser::parse_bitwise_and() {
    ParseResult result = parse_equality();
    if (!result.error.empty() || !result.parsed_hunk)
        return result;
    while (
        result.error.empty() &&
        current.ttype == TokenType::BITWISE_AND
    ) {
        Token op = consume();
        ParseResult right = parse_equality();
        if (right.parsed_hunk) {
            result.parsed_hunk = reinterpret_cast<TreeBase*>(
                new Bitwise{result.parsed_hunk, op, right.parsed_hunk}
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

ParseResult Parser::parse_equality() {
    ParseResult result = parse_comparison();
    if (!result.error.empty() || !result.parsed_hunk)
        return result;
    while (
        result.error.empty() &&
        check({TokenType::LOGICAL_EQUAL, TokenType::LOGICAL_NOT_EQUAL})
    ) {
        Token op = consume();
        ParseResult right = parse_comparison();
        if (right.parsed_hunk) {
            result.parsed_hunk = reinterpret_cast<TreeBase*>(
                new Equality{result.parsed_hunk, op, right.parsed_hunk}
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

ParseResult Parser::parse_comparison() {
    ParseResult result = parse_shift();
    if (!result.error.empty() || !result.parsed_hunk)
        return result;
    while (
        result.error.empty() &&
        check({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})
    ) {
        Token op = consume();
        ParseResult right = parse_shift();
        if (right.parsed_hunk) {
            result.parsed_hunk = reinterpret_cast<TreeBase*>(
                new Comparison{result.parsed_hunk, op, right.parsed_hunk}
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

ParseResult Parser::parse_shift() {
    ParseResult result = parse_term();
    if (!result.error.empty() || !result.parsed_hunk)
        return result;
    while (
        result.error.empty() &&
        check({TokenType::RIGHT_SHIFT, TokenType::LEFT_SHIFT})
    ) {
        Token op = consume();
        ParseResult right = parse_term();
        if (right.parsed_hunk) {
            result.parsed_hunk = reinterpret_cast<TreeBase*>(
                new Shift{result.parsed_hunk, op, right.parsed_hunk}
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

ParseResult Parser::parse_term() {
    ParseResult result = parse_factor();
    if (!result.error.empty() || !result.parsed_hunk)
        return result;
    while (
        result.error.empty() &&
        check({TokenType::PLUS, TokenType::MINUS})
    ) {
        Token op = consume();
        ParseResult right = parse_factor();
        if (right.parsed_hunk) {
            result.parsed_hunk = reinterpret_cast<TreeBase*>(
                new Term{result.parsed_hunk, op, right.parsed_hunk}
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

ParseResult Parser::parse_factor() {
    ParseResult result = parse_exponential();
    if (!result.error.empty() || !result.parsed_hunk)
        return result;
    while (
        result.error.empty() &&
        check({TokenType::STAR, TokenType::SLASH, TokenType::DOUBLE_SLASH, TokenType::PERCENT})
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
    std::vector<Token> ops;
    std::vector<TreeBase*> items;
    items.push_back(result.parsed_hunk);
    while (
        result.error.empty() &&
        current.ttype == TokenType::EXPONENT
    ) {
        Token op = consume();
        ParseResult exponent = parse_unary();
        if (exponent.parsed_hunk) {
            ops.push_back(op);
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
                    new Exponential{base, ops.back(), exponent}
                )
            );
            ops.pop_back();
        }
        result.parsed_hunk = items.back();
        items.pop_back();
    }
    return result;
}

ParseResult Parser::parse_unary() {
    if (!check({TokenType::BANG, TokenType::MINUS, TokenType::PLUS, TokenType::TILDE}))
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
    ParseResult result;
    if (current.ttype == TokenType::LINEBREAK) {
        read_next_token();
        if (current.ttype == TokenType::LINEBREAK)
            result.error.append("Expected expression");
        return result;
    }
    switch (current.ttype) {
        case TokenType::LEFT_ROUND_BRACE: {
            // Skip (
            read_next_token();
            if (current.is_type_keyword())
                result = parse_cast();
            else
                result = parse_group();
            if (!result.error.empty())
                result.parsed_hunk = nullptr;
            return result;
        }
        case TokenType::LEFT_CURLY_BRACE: {
            return parse_block();
        }
        case TokenType::IDENTIFIER: {
            Name* name_expr =
                new Name{consume().value};
            result.parsed_hunk =
                reinterpret_cast<TreeBase*>(name_expr);
            return result;
        }
        default: {}
    }
    return parse_literal();
}

ParseResult Parser::parse_literal() {
    TreeBase* parsed_hunk = nullptr;
    switch (current.ttype) {
        case TokenType::KEYWORD_VOID: {
            ObjectVoid* obj = ObjectVoid::VOID_OBJECT;
            Literal* void_literal =
                new Literal{reinterpret_cast<Object*>(obj)};
            read_next_token();
            parsed_hunk = reinterpret_cast<TreeBase*>(void_literal);
            break;
        }
        case TokenType::KEYWORD_TRUE: {
            ObjectBoolean* obj = ObjectBoolean::TRUE;
            Literal* bool_literal =
                new Literal{reinterpret_cast<Object*>(obj)};
            read_next_token();
            parsed_hunk = reinterpret_cast<TreeBase*>(bool_literal);
            break;
        }
        case TokenType::KEYWORD_FALSE: {
            ObjectBoolean* obj = ObjectBoolean::FALSE;
            Literal* bool_literal =
                new Literal{reinterpret_cast<Object*>(obj)};
            read_next_token();
            parsed_hunk = reinterpret_cast<TreeBase*>(bool_literal);
            break;
        }
        case TokenType::INTEGER: {
            ObjectInteger* obj = new ObjectInteger{std::stoll(consume().value)};
            Literal* int_literal =
                new Literal{reinterpret_cast<Object*>(obj)};
            parsed_hunk = reinterpret_cast<TreeBase*>(int_literal);
            break;
        }
        case TokenType::FLOAT: {
            ObjectFloat* obj = new ObjectFloat{std::stold(consume().value, nullptr)};
            Literal* float_literal =
                new Literal{reinterpret_cast<Object*>(obj)};
            parsed_hunk = reinterpret_cast<TreeBase*>(float_literal);
            break;
        }
        case TokenType::STRING: {
            ObjectString* obj = new ObjectString{consume().value};
            Literal* string_literal =
                new Literal{reinterpret_cast<Object*>(obj)};
            parsed_hunk = reinterpret_cast<TreeBase*>(string_literal);
            break;
        }
        default: {}
    }
    const std::string error{};
    return ParseResult{error, parsed_hunk};
}

ParseResult Parser::parse_block() {
    // Skip opening curly brace
    read_next_token();
    Block* block = new Block;
    if (current.ttype == TokenType::LINEBREAK) {
        block->opening_newline = new Token;
        *block->opening_newline = consume();
    }
    ParseResult result;
    while (has_next()) {
        result = parse_statement();
        if (!result.error.empty()) {
            result.parsed_hunk = nullptr;
            break;
        } else if (!result.parsed_hunk) {
            break;
        }
        block->statements.push_back(
            reinterpret_cast<Statement*>(result.parsed_hunk)
        );
    }
    if (current.ttype == TokenType::LINEBREAK) {
        block->closing_newline = new Token;
        *block->closing_newline = consume();
    }
    if (!result.error.empty()) {
        result.parsed_hunk = nullptr;
    } else if (current.ttype != TokenType::RIGHT_CURLY_BRACE) {
        result.parsed_hunk = nullptr;
        // Expected closing curly brace after statement
        result.error = "Expected \x7d after statement";
    } else {
        // Skip closing curly brace
        read_next_token();
        result.parsed_hunk =
            reinterpret_cast<TreeBase*>(block);
    }
    return result;
}

ParseResult Parser::parse_group() {
    ParseResult result = parse_expression();
    if (!result.parsed_hunk) {
        // Expected expression after opening round brace
        result.error = "Expected expression after \x28";
        return result;
    } else if (current.ttype != TokenType::RIGHT_ROUND_BRACE) {
        result.parsed_hunk = nullptr;
        // Expected closing round brace after statement
        result.error = "Expected \x29 after statement";
        return result;
    }
    // Skip closing round brace
    read_next_token();
    GroupedExpression* grouped_expr =
        new GroupedExpression{result.parsed_hunk};
    result.parsed_hunk =
        reinterpret_cast<TreeBase*>(grouped_expr);
    return result;
}

ParseResult Parser::parse_cast() {
    Token type_token = consume();
    Type* target_type =
        Type::get_type_by_token(type_token.ttype);
    if (!target_type) {
        std::cerr << "Undefined type '" << type_token.value << "'\n" ;
        exit(1);
    }
    // Skip closing round brace around target type
    read_next_token();
    ParseResult result = parse_primary();
    if (!result.error.empty()) {
        result.parsed_hunk = nullptr;
        return result;
    } else if (!result.parsed_hunk) {
        // Expected expression  after cast target type
        result.error = "Expected expression  after cast target type";
        return result;
    }
    Cast* cast_expr = new Cast{
        target_type,
        reinterpret_cast<TreeBase*>(result.parsed_hunk)
    };
    result.parsed_hunk =
        reinterpret_cast<TreeBase*>(cast_expr);
    return result;
}
