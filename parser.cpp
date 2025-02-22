#include <format>
#include <vector>
#include <utility>
#include "parser.hpp"
#include "object.hpp"
#include "syntax_tree.hpp"
#include "token.hpp"

void Parser::report_error(const std::string& s) noexcept {
    std::cerr << s << "\n\n" ;
}

void Parser::init(char* in, size_t source_len) noexcept {
    _errors = 0;
    lexer.init(in, source_len);
    read_next_token();
}

inline Lexer Parser::get_lexer() noexcept {
    return lexer;
}

inline void Parser::read_next_token() noexcept {
    current = lexer.generate_next_token();
}

inline bool Parser::is_at_end() const noexcept {
    return current.ttype == TokenType::END_OF_FILE;
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

void Parser::synchronize() noexcept {
    Token last = current;
    read_next_token();
    while (!is_at_end()) {
        if (
            last.ttype == TokenType::SEMI_COLON ||
            last.ttype == TokenType::RIGHT_CURLY_BRACE ||
            check({
                TokenType::KEYWORD_TYPE,
                TokenType::KEYWORD_VOID,
                TokenType::KEYWORD_BOOLEAN,
                TokenType::KEYWORD_STRING,
                TokenType::KEYWORD_FLOAT,
                TokenType::KEYWORD_INT,
                TokenType::KEYWORD_PRINT,
                TokenType::LEFT_CURLY_BRACE,
                TokenType::KEYWORD_RETURN
            })
        ) { return; }
        last = current;
        read_next_token();
    }
}

ParseResult Parser::parse_source() {
    // Skip empty lines
    while (current.ttype == TokenType::LINEBREAK)
        read_next_token();
    Program *source_tree = new Program;
    ParseResult result;
    while (!is_at_end()) {
        result = parse_statement();
        if (result.is_usable()) {
            source_tree->statements.push_back(
                reinterpret_cast<Statement*>(result.unwrap())
            );
        } else if (result.is_error()) {
            if (is_at_end()) break;
            report_error(result.unwrap_error());
            synchronize();
        } else if (result.is_null_value()) {
            continue;
        }
    }
    if (result.is_ok()) {
        result = ParseResult::Ok(
            _errors || source_tree->statements.empty() ? nullptr : source_tree
        );
    }
    return result;
}

ParseResult Parser::parse_statement() {
    ParseResult result;
    if (current.is_type_keyword())
        result = parse_variable_declaration();
    else if (check({TokenType::KEYWORD_PRINT}))
        result = parse_print();
    else
        result = parse_expression();
    if (result.is_usable()) {
        if (current.ttype == TokenType::SEMI_COLON) {
            // Consume ;
            read_next_token();
        } else if (
            is_mode_file() || !check(
                {TokenType::LINEBREAK, TokenType::SEMI_COLON, TokenType::END_OF_FILE}
            )
        ) {
            _errors++;
            result = ParseResult::Error("Expected ; after statement");
        }
    }
    return result;
}

ParseResult Parser::parse_print() {
    Print* print_stmt =
        new Print{current, nullptr};
    read_next_token();
    ParseResult result = parse_expression();
    if (result.is_ok()) {
        Expression* expr =
            reinterpret_cast<Expression*>(result.unwrap());
        if (expr) {
            print_stmt->expr = expr;
            result = ParseResult::Ok(print_stmt);
            return result;
        } else {
            _errors++;
            result = ParseResult::Error(
                "Expected expression after `print`"
            );
        }
    }
    report_error(result.unwrap_error());
    result = ParseResult::Ok(nullptr);
    synchronize();
    return result;
}

ParseResult Parser::parse_variable_declaration() {
    ParseResult result;
    Token type_token = consume();
    Type* target_type =
        Type::get_type_by_token(type_token.ttype);
    if (current.ttype != TokenType::IDENTIFIER) {
        _errors++;
        report_error(
            std::format(
                "Expected identifier after type {}",
                type_token.value
            )
        );
        synchronize();
        return ParseResult::Ok(nullptr);
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
                // Consume :=
                read_next_token();
                initializer = parse_expression();
                if (initializer.is_error()) {
                    result = ParseResult::Error(initializer.unwrap_error());
                    goto END;
                }
                initial_values.back().second =
                    initializer.unwrap();
                break;
            }
            case TokenType::COMMA: {
                // Consume ,
                read_next_token();
                break;
            }
            case TokenType::END_OF_FILE:
            case TokenType::SEMI_COLON: {
                goto END;
            }
            default: {
                result = ParseResult::Error("Unexpected item");
                goto END;
            }
        }
    }
END:
    if (result.is_ok()) {
        VariableDeclaration* declarations_list =
            new VariableDeclaration {target_type, initial_values};
        result = ParseResult::Ok(declarations_list);
    } else {
        _errors++;
        report_error(result.unwrap_error());
        result = ParseResult::Ok(nullptr);
        synchronize();
    }
    return result;
}

ParseResult Parser::parse_expression() {
    Token name_token = current;
    ParseResult result = parse_logical_or();
    if (result.is_error())
        return result;
    Name* name_expr =
        dynamic_cast<Name*>(result.unwrap());
    if (name_expr && current.ttype == TokenType::EQUAL) {
        // Consume assignment equal `=`
        read_next_token();
        Assignment* assignment = new Assignment{
            name_token, nullptr
        };
        ParseResult expr_result = parse_expression();
        if (expr_result.is_error())
            return expr_result;
        assignment->expr = reinterpret_cast<Expression*>(expr_result.unwrap());
        return ParseResult::Ok(assignment);
    }
    while (
        result.is_ok() &&
        current.ttype == TokenType::KEYWORD_XOR
    ) {
        Token op = consume();
        ParseResult right = parse_logical_or();
        if (right.is_usable()) {
            result = ParseResult::Ok(
                new Logical{result.unwrap(), op, right.unwrap()}
            );
        } else if (right.is_null_value()) {
            _errors++;
            result = ParseResult::Error(
                std::format("Expected expression after {}", op.value)
            );
            break;
        }
    }
    return result;
}

ParseResult Parser::parse_logical_or() {
    ParseResult result = parse_logical_and();
    while (
        result.is_ok() &&
        current.ttype == TokenType::KEYWORD_OR
    ) {
        Token op = consume();
        ParseResult right = parse_logical_and();
        if (right.is_usable()) {
            result = ParseResult::Ok(
                new Logical{result.unwrap(), op, right.unwrap()}
            );
        } else if (right.is_null_value()) {
            _errors++;
            result = ParseResult::Error(
                std::format("Expected expression after {}", op.value)
            );
            break;
        }
    }
    return result;
}

ParseResult Parser::parse_logical_and() {
    ParseResult result = parse_bitwise_xor();
    while (
        result.is_ok() &&
        current.ttype == TokenType::KEYWORD_AND
    ) {
        Token op = consume();
        ParseResult right = parse_bitwise_xor();
        if (right.is_usable()) {
            result = ParseResult::Ok(
                new Logical{result.unwrap(), op, right.unwrap()}
            );
        } else if (right.is_null_value()) {
            _errors++;
            result = ParseResult::Error(
                std::format("Expected expression after {}", op.value)
            );
            break;
        }
    }
    return result;
}

ParseResult Parser::parse_bitwise_xor() {
    ParseResult result = parse_bitwise_or();
    while (
        result.is_ok() &&
        current.ttype == TokenType::BITWISE_XOR
    ) {
        Token op = consume();
        ParseResult right = parse_bitwise_or();
        if (right.is_usable()) {
            result = ParseResult::Ok(
                new Bitwise{result.unwrap(), op, right.unwrap()}
            );
        } else if (right.is_null_value()) {
            _errors++;
            result = ParseResult::Error(
                std::format("Expected expression after {}", op.value)
            );
            break;
        }
    }
    return result;
}

ParseResult Parser::parse_bitwise_or() {
    ParseResult result = parse_bitwise_and();
    while (
        result.is_ok() &&
        current.ttype == TokenType::BITWISE_OR
    ) {
        Token op = consume();
        ParseResult right = parse_bitwise_and();
        if (right.is_usable()) {
            result = ParseResult::Ok(
                new Bitwise{result.unwrap(), op, right.unwrap()}
            );
        } else if (right.is_null_value()) {
            _errors++;
            result = ParseResult::Error(
                std::format("Expected expression after {}", op.value)
            );
            break;
        }
    }
    return result;
}

ParseResult Parser::parse_bitwise_and() {
    ParseResult result = parse_equality();
    while (
        result.is_ok() &&
        current.ttype == TokenType::BITWISE_AND
    ) {
        Token op = consume();
        ParseResult right = parse_equality();
        if (right.is_usable()) {
            result = ParseResult::Ok(
                new Bitwise{result.unwrap(), op, right.unwrap()}
            );
        } else if (right.is_null_value()) {
            _errors++;
            result = ParseResult::Error(
                std::format("Expected expression after {}", op.value)
            );
            break;
        }
    }
    return result;
}

ParseResult Parser::parse_equality() {
    ParseResult result = parse_comparison();
    while (
        result.is_ok() &&
        check({TokenType::LOGICAL_EQUAL, TokenType::LOGICAL_NOT_EQUAL})
    ) {
        Token op = consume();
        ParseResult right = parse_comparison();
        if (right.is_usable()) {
            result = ParseResult::Ok(
                new Equality{result.unwrap(), op, right.unwrap()}
            );
        } else if (right.is_null_value()) {
            _errors++;
            result = ParseResult::Error(
                std::format("Expected expression after {}", op.value)
            );
            break;
        }
    }
    return result;
}

ParseResult Parser::parse_comparison() {
    ParseResult result = parse_shift();
    while (
        result.is_ok() &&
        check({
            TokenType::GREATER,
            TokenType::GREATER_EQUAL,
            TokenType::LESS,
            TokenType::LESS_EQUAL
        })
    ) {
        Token op = consume();
        ParseResult right = parse_shift();
        if (right.is_usable()) {
            result = ParseResult::Ok(
                new Comparison{result.unwrap(), op, right.unwrap()}
            );
        } else if (right.is_null_value()) {
            _errors++;
            result = ParseResult::Error(
                std::format("Expected expression after {}", op.value)
            );
            break;
        }

    }
    return result;
}

ParseResult Parser::parse_shift() {
    ParseResult result = parse_term();
    while (
        result.is_ok() &&
        check({TokenType::RIGHT_SHIFT, TokenType::LEFT_SHIFT})
    ) {
        Token op = consume();
        ParseResult right = parse_term();
        if (right.is_usable()) {
            result = ParseResult::Ok(
                new Shift{result.unwrap(), op, right.unwrap()}
            );
        } else if (right.is_null_value()) {
            _errors++;
            result = ParseResult::Error(
                std::format("Expected expression after {}", op.value)
            );
            break;
        }

    }
    return result;
}

ParseResult Parser::parse_term() {
    ParseResult result = parse_factor();
    while (
        result.is_ok() &&
        check({TokenType::PLUS, TokenType::MINUS})
    ) {
        Token op = consume();
        ParseResult right = parse_factor();
        if (right.is_usable()) {
            result = ParseResult::Ok(
                new Term{result.unwrap(), op, right.unwrap()}
            );
        } else if (right.is_null_value()) {
            _errors++;
            result = ParseResult::Error(
                std::format("Expected expression after {}", op.value)
            );
            break;
        }

    }
    return result;
}

ParseResult Parser::parse_factor() {
    ParseResult result = parse_exponential();
    while (
        result.is_ok() && check({
            TokenType::STAR,
            TokenType::SLASH,
            TokenType::DOUBLE_SLASH,
            TokenType::PERCENT
        })
    ) {
        Token op = consume();
        ParseResult right = parse_exponential();
        if (right.is_usable()) {
            result = ParseResult::Ok(
                new Factor{result.unwrap(), op, right.unwrap()}
            );
        } else if (right.is_null_value()) {
            _errors++;
            result = ParseResult::Error(
                std::format("Expected expression after {}", op.value)
            );
            break;
        }
    }
    return result;
}

ParseResult Parser::parse_exponential() {
    ParseResult result = parse_unary();
    if (result.is_useless())
        return result;
    std::vector<Token> ops;
    std::vector<TreeBase*> items;
    items.push_back(result.unwrap());
    while (
        result.is_ok() &&
        current.ttype == TokenType::EXPONENT
    ) {
        Token op = consume();
        ParseResult exponent = parse_unary();
        if (exponent.is_error()) {
            result = ParseResult::Error(
                exponent.unwrap_error()
            );
        } else if (exponent.is_usable()) {
            ops.push_back(op);
            items.push_back(exponent.unwrap());
        } else if (result.is_null_value()) {
            _errors++;
            result = ParseResult::Error(
                "Expected expression after **"
            );
            break;
        }
    }
    if (result.is_ok()) {
        while (items.size() >= 2) {
            TreeBase* exponent = items.back();
            items.pop_back();
            TreeBase* base = items.back();
            items.pop_back();
            items.push_back(
                new Exponential{base, ops.back(), exponent}
            );
            ops.pop_back();
        }
        result = ParseResult::Ok(items.back());
        items.pop_back();
        return result;
    }
    return ParseResult::Ok(nullptr);
}

ParseResult Parser::parse_unary() {
    if (!check({
        TokenType::BANG,
        TokenType::MINUS,
        TokenType::PLUS,
        TokenType::TILDE
    })) {
        return parse_primary();
    }
    ParseResult result;
    Token op = consume();
    result = parse_unary();
    if (result.is_usable()) {
        Unary* unary = new Unary{op, result.unwrap()};
        result = ParseResult::Ok(unary);
    } else if (result.is_null_value()) {
        _errors++;
        result = ParseResult::Error(
            std::format("Expected expression after ", op.value)
        );
    }
    return result;
}

ParseResult Parser::parse_primary() {
    ParseResult result;
    if (current.ttype == TokenType::LINEBREAK) {
        // Consume \n
        read_next_token();
        if (current.ttype == TokenType::LINEBREAK) {
            _errors++;
            result = ParseResult::Error("Expected expression");
        }
        return result;
    }
    switch (current.ttype) {
        case TokenType::LEFT_ROUND_BRACE: {
            // Consume (
            read_next_token();
            if (current.is_type_keyword())
                result = parse_cast();
            else
                result = parse_group();
            break;
        }
        case TokenType::LEFT_CURLY_BRACE: {
            result = parse_block();
            break;
        }
        case TokenType::IDENTIFIER: {
            Name* name_expr =
                new Name{consume().value};
            result = ParseResult::Ok(name_expr);
            break;
        }
        default: {
            result = parse_literal();
        }
    }
    return result;
}

ParseResult Parser::parse_literal() {
    TreeBase* parsed_hunk = nullptr;
    switch (current.ttype) {
        case TokenType::KEYWORD_VOID: {
            ObjectVoid* obj = ObjectVoid::VOID_OBJECT;
            parsed_hunk =
                new Literal{reinterpret_cast<Object*>(obj)};
            read_next_token();
            break;
        }
        case TokenType::KEYWORD_TRUE: {
            ObjectBoolean* obj = ObjectBoolean::TRUE;
            parsed_hunk =
                new Literal{reinterpret_cast<Object*>(obj)};
            read_next_token();
            break;
        }
        case TokenType::KEYWORD_FALSE: {
            ObjectBoolean* obj = ObjectBoolean::FALSE;
            parsed_hunk =
                new Literal{reinterpret_cast<Object*>(obj)};
            read_next_token();
            break;
        }
        case TokenType::INTEGER: {
            ObjectInteger* obj = new ObjectInteger{std::stoll(consume().value)};
            parsed_hunk =
                new Literal{reinterpret_cast<Object*>(obj)};
            break;
        }
        case TokenType::FLOAT: {
            ObjectFloat* obj = new ObjectFloat{std::stold(consume().value, nullptr)};
            parsed_hunk =
                new Literal{reinterpret_cast<Object*>(obj)};
            break;
        }
        case TokenType::STRING: {
            ObjectString* obj = new ObjectString{consume().value};
            parsed_hunk =
                new Literal{reinterpret_cast<Object*>(obj)};
            break;
        }
        default: {
            return ParseResult::Ok(nullptr);
        }
    }
    return ParseResult::Ok(parsed_hunk);
}

ParseResult Parser::parse_block() {
    // Skip opening curly brace
    read_next_token();
    Block* block = new Block;
    ParseResult result;
    while (current.ttype != TokenType::RIGHT_CURLY_BRACE) {
        if (current.ttype == TokenType::KEYWORD_RETURN)
            result = parse_return();
        else
            result = parse_statement();
        if (result.is_usable()) {
            block->statements.push_back(
                reinterpret_cast<Statement*>(result.unwrap())
            );
        } else if (result.is_error()) {
            report_error(result.unwrap_error());
            synchronize();
        } else if (result.is_null_value()) {
            continue;
        }
    }
    if (result.is_ok()) {
        if (current.ttype == TokenType::RIGHT_CURLY_BRACE) {
            // Skip closing curly brace
            read_next_token();
            result = ParseResult::Ok(block);
            return result;
        } else {
            // Expected closing curly brace after statement
            _errors++;
            result = ParseResult::Error(
                "Expected \x7d after statement"
            );
        }
    }
    report_error(result.unwrap_error());
    result = ParseResult::Ok(nullptr);
    synchronize();
    return result;
}

ParseResult Parser::parse_return() {
    // Skip keyword `return`
    read_next_token();
    ParseResult result = parse_expression();
    if (result.is_ok()) {
        if (current.ttype == TokenType::SEMI_COLON) {
            // Skip ;
            read_next_token();
            Return* ret = new Return{
                reinterpret_cast<Expression*>(result.unwrap())
            };
            result = ParseResult::Ok(ret);
            return result;
        } else {
            _errors++;
            result = ParseResult::Error(
                "Expected ; after statement"
            );
        }
    }
    report_error(result.unwrap_error());
    result = ParseResult::Ok(nullptr);
    synchronize();
    return result;
}

ParseResult Parser::parse_group() {
    ParseResult result = parse_expression();
    if (result.is_usable()) {
        if (current.ttype == TokenType::RIGHT_ROUND_BRACE) {
            // Skip closing round brace
            read_next_token();
            GroupedExpression* grouped_expr =
                new GroupedExpression{result.unwrap()};
            result = ParseResult::Ok(grouped_expr);
        } else {
            // Expected closing round brace after statement
            _errors++;
            result = ParseResult::Error(
                "Expected \x29 after statement"
            );
        }
    } else if (result.is_null_value()) {
        // Expected expression after opening round brace
        _errors++;
        result = ParseResult::Error(
            "Expected expression after \x28"
        );
    }
    return result;
}

ParseResult Parser::parse_cast() {
    Token type_token = consume();
    Type* target_type =
        Type::get_type_by_token(type_token.ttype);
    if (!target_type) {
        _errors++;
        return ParseResult::Error(
            std::format("Undefined type `{}`", type_token.value)
        );
    }
    // Skip closing round brace around target type
    read_next_token();
    ParseResult result = parse_primary();
    if (result.is_usable()) {
        Cast* cast_expr = new Cast{
            target_type,
            reinterpret_cast<Expression*>(result.unwrap())
        };
        result = ParseResult::Ok(cast_expr);
    } else if (result.is_null_value()) {
        // Expected expression after cast target type
        _errors++;
        result = ParseResult::Error(
            "Expected expression after cast target type"
        );
    }
    return result;
}
