#include "parser.hpp"
#include "object.hpp"
#include "syntax_tree.hpp"
#include "token.hpp"

void Parser::report_error(const ErrorPair& error_pair) const noexcept {
    std::cerr << std::format(
        "\033[36m{}:{}:{}:\033[0m \033[31merror:\033[0m {}\n{}\n",
        *Common::get_filename(),
        last_used.col+last_used.value.length()+1, last_used.line+1,
        error_pair.msg,
        error_pair.diagnostics
    );
}

void Parser::init(char* in, size_t source_len) noexcept {
    _errors = 0;
    lexer.init(in, source_len);
    read_next_token();
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
        if (current.ttype == TokenType::LINEBREAK)
            goto SKIP;
        else if (
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
                TokenType::KEYWORD_RETURN,
                TokenType::LEFT_CURLY_BRACE
            })
        ) { return; }
    SKIP:
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
    else if (current.ttype == TokenType::KEYWORD_PRINT)
        result = parse_print();
    else
        result = parse_expression();
    if (result.is_usable()) {
        if (current.ttype == TokenType::SEMI_COLON) {
            last_used = current;
            // Consume ;
            read_next_token();
        } else if (
            Common::is_mode_file() || !check({
                TokenType::LINEBREAK,
                TokenType::SEMI_COLON,
                TokenType::END_OF_FILE
            })
        ) {
            _errors++;
            const std::string msg{"Expected ; after statement"};
            std::string current_line =
                lexer.lines.at(last_used.line);
            std::string header =
                std::format("{:6} | ", last_used.line+1);
            std::string::size_type end =
                std::min(
                    last_used.col + last_used.value.length(),
                    current_line.length()
                );
            if (end < current_line.length() && !std::isspace(current_line.at(end))) {
                current_line =
                    current_line.substr(0, end) + ' ' + current_line.substr(end);
            }
            std::string diagnostics =
                std::format(
                    "{}{}\n{}{}",
                    header,
                    current_line,
                    std::string(header.length() + end, ' '),
                    "\033[32m^\033[0m" // a green caret
                );
            result = ParseResult::Error(ErrorPair{msg, diagnostics});
        }
    }
    return result;
}

ParseResult Parser::parse_print() {
    Print* print_stmt =
        new Print{nullptr};
    last_used = current;
    // Skip keyword `print`
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
            const std::string msg{"Expected expression after `print`"};
            std::string current_line =
                lexer.lines.at(last_used.line);
            std::string header =
                std::format("{:6} | ", last_used.line+1);
            std::string::size_type end =
                std::min(
                    last_used.col + last_used.value.length(),
                    current_line.length()
                );
            std::string diagnostics =
                std::format(
                    "{}{} {}\n{}{}",
                    header,
                    current_line.substr(0, end),
                    current_line.substr(end),
                    std::string(header.length() + end + 1, ' '),
                    "\033[32m^\033[0m" // a green caret
                );
            result = ParseResult::Error(ErrorPair{msg, diagnostics});
        }
    }
    report_error(result.unwrap_error());
    result = ParseResult::Ok(nullptr);
    synchronize();
    return result;
}

ParseResult Parser::parse_variable_declaration() {
    ParseResult result;
    last_used = current;
    Token type_token = consume();
    Type* target_type =
        Type::get_type_by_token(type_token.ttype);
    if (!target_type) {
        _errors++;
        result = ParseResult::Error(
            ErrorPair{
                std::format("Undefined type `{}`", type_token.value),
                std::string{}
            }
        );
    } else if (current.ttype != TokenType::IDENTIFIER) {
        _errors++;
        const std::string msg{"Unexpected item, expected identifier after type"};
        std::string current_line =
            lexer.lines.at(last_used.line);
        std::string header =
            std::format("{:6} | ", last_used.line+1);
        std::string::size_type end =
            std::min(
                last_used.col + last_used.value.length(),
                current_line.length()
            );
        std::string diagnostics =
            std::format(
                "{}{}\n{}{}",
                header,
                current_line,
                std::string(header.length() + end + 1, ' '),
                std::format( // a bunch of red carets
                    "\033[31m{}\033[0m", std::string(current.value.length(), '^')
                )
            );
        result = ParseResult::Error(ErrorPair{msg, diagnostics});
    }
    VariableDeclaration::var_value_pairs initial_values;
    ParseResult initializer;
    while (result.is_ok()) {
        switch (current.ttype) {
            case TokenType::IDENTIFIER: {
                last_used = current;
                initial_values.push_back(
                    std::make_pair(
                        std::move(consume().value),
                        nullptr
                    )
                );
                break;
            }
            case TokenType::COLON_EQUAL: {
                last_used = current;
                // Consume :=
                read_next_token();
                initializer = parse_expression();
                if (initializer.is_error()) {
                    result = ParseResult::Error(
                        initializer.unwrap_error()
                    );
                } else {
                    initial_values.back().second =
                        initializer.unwrap();
                }
                break;
            }
            case TokenType::COMMA: {
                last_used = current;
                // Consume ,
                read_next_token();
                break;
            }
            case TokenType::SEMI_COLON: {
                break;
            }
            default: {
                _errors++;
                result = ParseResult::Error(ErrorPair{"Unexpected item""Unexpected item", std::string{}});
                goto END;
            }
        }
    }
    if (result.is_ok()) {
        VariableDeclaration* declarations_list =
            new VariableDeclaration {target_type, initial_values};
        result = ParseResult::Ok(declarations_list);
    } else {
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
        last_used = current;
        // Consume assignment equal `=`
        read_next_token();
        Assignment* assignment = new Assignment{
            name_token, nullptr
        };
        ParseResult expr_result = parse_expression();
        if (expr_result.is_error())
            return expr_result;
        assignment->expr =
            reinterpret_cast<Expression*>(expr_result.unwrap());
        return ParseResult::Ok(assignment);
    }
    while (
        result.is_ok() &&
        current.ttype == TokenType::KEYWORD_XOR
    ) {
        last_used = current;
        Token op = consume();
        ParseResult right = parse_logical_or();
        if (right.is_usable()) {
            result = ParseResult::Ok(
                new Logical{result.unwrap(), op, right.unwrap()}
            );
        } else if (right.is_null_value()) {
            _errors++;
            result = ParseResult::Error(
                ErrorPair{std::format("Expected expression after {}", op.value), std::string{}}
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
        last_used = current;
        Token op = consume();
        ParseResult right = parse_logical_and();
        if (right.is_usable()) {
            result = ParseResult::Ok(
                new Logical{result.unwrap(), op, right.unwrap()}
            );
        } else if (right.is_null_value()) {
            _errors++;
            result = ParseResult::Error(
                ErrorPair{std::format("Expected expression after {}", op.value), std::string{}}
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
        last_used = current;
        Token op = consume();
        ParseResult right = parse_bitwise_xor();
        if (right.is_usable()) {
            result = ParseResult::Ok(
                new Logical{result.unwrap(), op, right.unwrap()}
            );
        } else if (right.is_null_value()) {
            _errors++;
            result = ParseResult::Error(
                ErrorPair{std::format("Expected expression after {}", op.value), std::string{}}
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
        last_used = current;
        Token op = consume();
        ParseResult right = parse_bitwise_or();
        if (right.is_usable()) {
            result = ParseResult::Ok(
                new Bitwise{result.unwrap(), op, right.unwrap()}
            );
        } else if (right.is_null_value()) {
            _errors++;
            result = ParseResult::Error(
                ErrorPair{std::format("Expected expression after {}", op.value), std::string{}}
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
        last_used = current;
        Token op = consume();
        ParseResult right = parse_bitwise_and();
        if (right.is_usable()) {
            result = ParseResult::Ok(
                new Bitwise{result.unwrap(), op, right.unwrap()}
            );
        } else if (right.is_null_value()) {
            _errors++;
            result = ParseResult::Error(
                ErrorPair{std::format("Expected expression after {}", op.value), std::string{}}
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
        last_used = current;
        Token op = consume();
        ParseResult right = parse_equality();
        if (right.is_usable()) {
            result = ParseResult::Ok(
                new Bitwise{result.unwrap(), op, right.unwrap()}
            );
        } else if (right.is_null_value()) {
            _errors++;
            result = ParseResult::Error(
                ErrorPair{std::format("Expected expression after {}", op.value), std::string{}}
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
        last_used = current;
        Token op = consume();
        ParseResult right = parse_comparison();
        if (right.is_usable()) {
            result = ParseResult::Ok(
                new Equality{result.unwrap(), op, right.unwrap()}
            );
        } else if (right.is_null_value()) {
            _errors++;
            result = ParseResult::Error(
                ErrorPair{std::format("Expected expression after {}", op.value), std::string{}}
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
        last_used = current;
        Token op = consume();
        ParseResult right = parse_shift();
        if (right.is_usable()) {
            result = ParseResult::Ok(
                new Comparison{result.unwrap(), op, right.unwrap()}
            );
        } else if (right.is_null_value()) {
            _errors++;
            result = ParseResult::Error(
                ErrorPair{std::format("Expected expression after {}", op.value), std::string{}}
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
        last_used = current;
        Token op = consume();
        ParseResult right = parse_term();
        if (right.is_usable()) {
            result = ParseResult::Ok(
                new Shift{result.unwrap(), op, right.unwrap()}
            );
        } else if (right.is_null_value()) {
            _errors++;
            result = ParseResult::Error(
                ErrorPair{std::format("Expected expression after {}", op.value), std::string{}}
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
        last_used = current;
        Token op = consume();
        ParseResult right = parse_factor();
        if (right.is_usable()) {
            result = ParseResult::Ok(
                new Term{result.unwrap(), op, right.unwrap()}
            );
        } else if (right.is_null_value()) {
            _errors++;
            result = ParseResult::Error(
                ErrorPair{std::format("Expected expression after {}", op.value), std::string{}}
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
        last_used = current;
        Token op = consume();
        ParseResult right = parse_exponential();
        if (right.is_usable()) {
            result = ParseResult::Ok(
                new Factor{result.unwrap(), op, right.unwrap()}
            );
        } else if (right.is_null_value()) {
            _errors++;
            result = ParseResult::Error(
                ErrorPair{std::format("Expected expression after {}", op.value), std::string{}}
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
        last_used = current;
        Token op = consume();
        ParseResult exponent = parse_unary();
        if (exponent.is_error()) {
            result = ParseResult::Error(
                exponent.unwrap_error()
            );
            break;
        } else if (exponent.is_usable()) {
            ops.push_back(op);
            items.push_back(exponent.unwrap());
        } else if (result.is_null_value()) {
            _errors++;
            result = ParseResult::Error(
                ErrorPair{"Expected expression after **", std::string{}}
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
    })) { return parse_primary(); }
    ParseResult result;
    last_used = current;
    Token op = consume();
    result = parse_unary();
    if (result.is_usable()) {
        Unary* unary = new Unary{op, result.unwrap()};
        result = ParseResult::Ok(unary);
    } else if (result.is_null_value()) {
        _errors++;
        result = ParseResult::Error(
            ErrorPair{std::format("Expected expression after {}", op.value), std::string{}}
        );
    }
    return result;
}

ParseResult Parser::parse_primary() {
    ParseResult result;
    switch (current.ttype) {
        case TokenType::LEFT_ROUND_BRACE: {
            last_used = current;
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
            last_used = current;
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
            break;
        }
        case TokenType::KEYWORD_TRUE: {
            ObjectBoolean* obj = ObjectBoolean::TRUE;
            parsed_hunk =
                new Literal{reinterpret_cast<Object*>(obj)};
            break;
        }
        case TokenType::KEYWORD_FALSE: {
            ObjectBoolean* obj = ObjectBoolean::FALSE;
            parsed_hunk =
                new Literal{reinterpret_cast<Object*>(obj)};
            break;
        }
        case TokenType::INTEGER: {
            ObjectInteger* obj = new ObjectInteger{std::stoll(current.value)};
            parsed_hunk =
                new Literal{reinterpret_cast<Object*>(obj)};
            break;
        }
        case TokenType::FLOAT: {
            ObjectFloat* obj = new ObjectFloat{std::stold(current.value, nullptr)};
            parsed_hunk =
                new Literal{reinterpret_cast<Object*>(obj)};
            break;
        }
        case TokenType::STRING: {
            ObjectString* obj = new ObjectString{current.value};
            parsed_hunk =
                new Literal{reinterpret_cast<Object*>(obj)};
            break;
        }
        default: {
            return ParseResult::Ok(nullptr);
        }
    }
    last_used = current;
    read_next_token();
    return ParseResult::Ok(parsed_hunk);
}

ParseResult Parser::parse_block() {
    last_used = current;
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
            last_used = current;
            // Skip closing curly brace
            read_next_token();
            result = ParseResult::Ok(block);
            return result;
        } else {
            // Expected closing curly brace after statement
            _errors++;
            result = ParseResult::Error(
                ErrorPair{"Expected \x7d after statement", std::string{}}
            );
        }
    }
    report_error(result.unwrap_error());
    result = ParseResult::Ok(nullptr);
    synchronize();
    return result;
}

ParseResult Parser::parse_return() {
    last_used = current;
    // Skip keyword `return`
    read_next_token();
    ParseResult result = parse_expression();
    if (result.is_ok()) {
        if (current.ttype == TokenType::SEMI_COLON) {
            last_used = current;
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
                ErrorPair{"Expected ; after statement", std::string{}}
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
            last_used = current;
            // Skip closing round brace
            read_next_token();
            GroupedExpression* grouped_expr =
                new GroupedExpression{result.unwrap()};
            result = ParseResult::Ok(grouped_expr);
        } else {
            // Expected closing round brace after statement
            _errors++;
            result = ParseResult::Error(
                ErrorPair{"Expected \x29 after expression", std::string{}}
            );
        }
    } else if (result.is_null_value()) {
        // Expected expression after opening round brace
        _errors++;
        result = ParseResult::Error(
            ErrorPair{"Expected expression after \x28", std::string{}}
        );
    }
    return result;
}

ParseResult Parser::parse_cast() {
    last_used = current;
    Token type_token = consume();
    Type* target_type =
        Type::get_type_by_token(type_token.ttype);
    if (!target_type) {
        _errors++;
        return ParseResult::Error(
            ErrorPair{std::format("Undefined type `{}`", type_token.value), std::string{}}
        );
    }
    last_used = current;
    if (current.ttype != TokenType::RIGHT_ROUND_BRACE) {
        _errors++;
        return ParseResult::Error(
            ErrorPair{"Expected \x29 after cast type", std::string{}}
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
            ErrorPair{"Expected expression after cast target type", std::string{}}
        );
    }
    return result;
}
