#include <cctype>
#include "lexer.hpp"

void Lexer::init(char* in, const size_t& source_len) {
    errors = 0;
    source.assign(in, source_len);
    if (source.back() != '\n')
        source.push_back('\n');
    current = source.begin();
    lines.push_back(std::string{});
}

inline bool Lexer::is_at_end() {
    return current == source.end();
}

inline void Lexer::skip_whitespaces() {
    while (!is_at_end() && std::isspace(*current)) {
        if (current != source.begin() && *(current-1) == '\n') {
            // Add a new line
            lines.push_back(std::string{});
            col = 0;
        }
        current++;
        col++;
    }
    if (is_at_end()) {
        std::string::size_type last_line_break_in_input =
            source.rfind('\n');
        lines.back().assign(
            source.substr(last_line_break_in_input)
        );
    } else if (lines.back().empty()) {
        std::string::size_type cur_pos =
            std::distance(source.cbegin(), current);
        std::string::size_type begin =
            source.find_last_of('\n', cur_pos);
        if (begin == std::string::npos)
            begin = 0;
        std::string::size_type end =
            source.find_first_of('\n', cur_pos);
        std::string::size_type length = end - begin;
        if (length > 0)
            length--;
        lines.back().assign(
            source.substr(begin, length)
        );
    }
}

static bool is_valid_first_char(char c) {
    return (
        std::isalpha(c) || c == '\n' ||
        c == '{' || c == '}' || c == '(' || c == ')' ||
        c == '!' || c == '=' || c == '-' || c == '+' ||
        c == '*' || c == '/' || c == '%' || c == '<' ||
        c == '>' || c == '~' || c == '|' || c == '&' ||
        c == '^' || c == ':' || c == ',' || c == ';' ||
        c == '_' || c == '"'
    );
}

Token Lexer::generate_number_token() {
    std::string::const_iterator token_start = current;
    TokenType ttype;
    std::string value;
    // Generate a INTEGER/FLOAT token
    // Roughly it's: \d+([.]\d+((e|E)[+-]?\d+)?)?
    ttype = TokenType::INTEGER;

    // Read digits before decimal point before end of input
    while (!is_at_end() && std::isdigit(*current)) {
        value.push_back(*current);
        current++;
    }

    if (is_at_end() || *current != '.') {
        // We found an integer
        goto RETURN_TOKEN;
    }

    // Read decimal point
    value.push_back('.');
    // Move over decimal point
    current++;

    if (is_at_end() || !std::isdigit(*current)) {
        // Invalid numeric literal: no digits after decimal point
        std::string repr;
        repr += std::string(std::distance(source.cbegin(), token_start), ' ') ;
        repr += std::string(value.size(), '^') ;
        Lexer::report_lexing_error(
            "Invalid numeric literal: no digits after decimal point",
            repr
        );
        ttype = TokenType::INVALID;
        goto RETURN_TOKEN;
    }

    ttype = TokenType::FLOAT;
    // Read digits after decimal point before end of input
    while (!is_at_end() && std::isdigit(*current)) {
        value.push_back(*current);
        current++;
    }

    if (is_at_end() || !(*current == 'e' || *current == 'E')) {
        // No exponent
        goto RETURN_TOKEN;
    }

    // Push e
    value.push_back(*current);
    // Move over e
    current++;

    if (is_at_end() || !(std::isdigit(*current) || *current == '+' || *current == '-')) {
        // Invalid numeric literal: Missing exponent value
        std::string repr;
        repr += std::string(std::distance(source.cbegin(), token_start), ' ') ;
        repr += std::string(value.size(), '^') ;
        Lexer::report_lexing_error(
            "Invalid numeric literal: Missing exponent value",
            repr
        );
        ttype = TokenType::INVALID;
        goto RETURN_TOKEN;
    }

    // Push exponent sign or first digit after e/E
    value.push_back(*current);
    // Move over
    current++;

    // Read digits after exponent sign
    while (!is_at_end() && std::isdigit(*current)) {
        value.push_back(*current);
        current++;
    }

RETURN_TOKEN:
    return Token{ttype, value, col, lines.size()-1};
}

Token Lexer::generate_string_token() {
    std::string::const_iterator token_start = current;
    TokenType ttype;
    std::string value;
    bool terminated = false;
    // Skip opening "
    current++;
    ttype = TokenType::STRING;
    std::vector<size_t> invalid_escapes;
    for (;;current++) {
        if (is_at_end() || *current == '\n') {
            break;
        } else if (*current == '"') {
            // Skip closing "
            current++;
            terminated = true;
            break;
        } else if (*current == '\\') {
            // Move to escaped character
            current++;
            if (is_at_end()) {
                break;
            } else if (*current == '\n') {
                continue;
            } else if (*current == '\\') {
                // Quoted slash
                value.push_back('\\');
            } else if (*current == '"') {
                // Quoted "
                value.push_back('"');
            } else if (*current == '0') {
                // Null character
                value.push_back('\0');
            } else if (*current == 'a') {
                value.push_back('\a');
            } else if (*current == 'b') {
                value.push_back('\b');
            } else if (*current == 'f') {
                value.push_back('\f');
            } else if (*current == 'n') {
                value.push_back('\n');
            } else if (*current == 'r') {
                value.push_back('\r');
            } else if (*current == 't') {
                value.push_back('\t');
            } else if (*current == 'v') {
                value.push_back('\v');
            } else {
                if (!std::isspace(*current)) {
                    size_t pos =
                        static_cast<size_t>(std::distance(source.cbegin(), current));
                    if (pos > 0) pos--;
                    invalid_escapes.push_back(pos);
                }
                value.push_back('\\');
                value.push_back(*current);
            }
        } else {
            // Any other character
            // just append it
            value.push_back(*current);
        }
    }
    if (!invalid_escapes.empty()) {
        std::string repr;
        ttype = TokenType::INVALID;
        size_t i = 0;
        for (auto pos : invalid_escapes) {
            while (i < pos) {
                repr += ' ' ;
                i++;
            }
            repr += "^^" ;
            i = pos + 2;
        }
        Lexer::report_lexing_error(
            "Invalid escape sequence",
            repr
        );
    }
    if (!terminated) {
        std::string repr;
        ttype = TokenType::INVALID;
        repr += std::string(std::distance(source.cbegin(), token_start), ' ') ;
        repr += std::string(value.size(), '^') ;
        Lexer::report_lexing_error(
            "Unterminated string literal",
            repr
        );
    }
    return Token{ttype, value, col, lines.size()-1};
}

Token Lexer::generate_identifier_token() {
    TokenType ttype = TokenType::IDENTIFIER;
    std::string value;
    while (!is_at_end() && (*current == '_' || std::isalpha(*current))) {
        value.push_back(*current);
        current++;
    }
    if (value == "and")
        ttype = TokenType::KEYWORD_AND;
    else if (value == "boolean")
        ttype = TokenType::KEYWORD_BOOLEAN;
    else if (value == "false")
        ttype = TokenType::KEYWORD_FALSE;
    else if (value == "float")
        ttype = TokenType::KEYWORD_FLOAT;
    else if (value == "int")
        ttype = TokenType::KEYWORD_INT;
    else if (value == "or")
        ttype = TokenType::KEYWORD_OR;
    else if (value == "print")
        ttype = TokenType::KEYWORD_PRINT;
    else if (value == "string")
        ttype = TokenType::KEYWORD_STRING;
    else if (value == "return")
        ttype = TokenType::KEYWORD_RETURN;
    else if (value == "true")
        ttype = TokenType::KEYWORD_TRUE;
    else if (value == "void")
        ttype = TokenType::KEYWORD_VOID;
    else if (value == "xor")
        ttype = TokenType::KEYWORD_XOR;
    else if (value == "type")
        ttype = TokenType::KEYWORD_TYPE;
    return Token{ttype, value, col, lines.size()-1};
}

Token Lexer::generate_invalid_token() {
    // Any other non-whitespace character
    const TokenType ttype = TokenType::INVALID;
    std::string value;
    while (
        !is_at_end() &&
        !is_valid_first_char(*current)
    ) {
        value.push_back(*current);
        current++;
    }
    return Token{ttype, value, col, lines.size()-1};
}

Token Lexer::generate_next_token() {
    size_t old_line = lines.size();
    TokenType ttype;
    std::string value;
    skip_whitespaces();
    if (is_at_end()) {
        return Token{
            TokenType::END_OF_FILE,
            std::string(),
            col,
            lines.size()-1
        };
    } else if (old_line < lines.size()) {
        return Token{
            TokenType::LINEBREAK,
            std::string("\n"),
            col,
            lines.size()-1
        };
    }
    switch (*current) {
        case ';':
            current++;
            ttype = TokenType::SEMI_COLON;
            value = ";";
            break;
        case ',':
            current++;
            ttype = TokenType::COMMA;
            value = ",";
            break;
        case ':':
            current++;
            if (!is_at_end() && *current == '=') {
                current++;
                ttype = TokenType::COLON_EQUAL;
                value = ":=";
                break;
            } else {
                current--;
                Token tok = generate_invalid_token();
                col += tok.value.length();
                return tok;
            }
        case '{':
            current++;
            ttype = TokenType::LEFT_CURLY_BRACE;
            value = "{";
            break;
        case '}':
            current++;
            ttype = TokenType::RIGHT_CURLY_BRACE;
            value = "}";
            break;
        case '(':
            current++;
            ttype = TokenType::LEFT_ROUND_BRACE;
            value = "(";
            break;
        case ')':
            current++;
            ttype = TokenType::RIGHT_ROUND_BRACE;
            value = ")";
            break;
        case '!':
            current++;
            if (!is_at_end() && *current == '=') {
                current++;
                ttype = TokenType::LOGICAL_NOT_EQUAL;
                value = "!=";
            } else {
                ttype = TokenType::BANG;
                value = "!";
            }
            break;
        case '=':
            current++;
            if (!is_at_end() && *current == '=') {
                current++;
                ttype = TokenType::LOGICAL_EQUAL;
                value = "==";
            } else {
                ttype = TokenType::EQUAL;
                value = "=";
            }
            break;
        case '&':
            current++;
            ttype = TokenType::BITWISE_AND;
            value = "&";
            break;
        case '|':
            current++;
            ttype = TokenType::BITWISE_OR;
            value = "|";
            break;
        case '^':
            current++;
            ttype = TokenType::BITWISE_XOR;
            value = "^";
            break;
        case '-':
            current++;
            ttype = TokenType::MINUS;
            value = "-";
            break;
        case '*':
            current++;
            if (!is_at_end() && *current == '*') {
                current++;
                ttype = TokenType::EXPONENT;
                value = "**";
            } else {
                ttype = TokenType::STAR;
                value = "*";
            }
            break;
        case '/':
            current++;
            if (!is_at_end() && *current == '/') {
                current++;
                ttype = TokenType::DOUBLE_SLASH;
                value = "//";
            } else {
                ttype = TokenType::SLASH;
                value = "/";
            }
            break;
        case '%':
            current++;
            ttype = TokenType::PERCENT;
            value = "%";
            break;
        case '+':
            current++;
            ttype = TokenType::PLUS;
            value = "+";
            break;
        case '>':
            current++;
            if (!is_at_end()) {
                if (*current == '=') {
                    current++;
                    ttype = TokenType::GREATER_EQUAL;
                    value = ">=";
                    goto RETURN_TOKEN;
                } else if (*current == '>') {
                    current++;
                    ttype = TokenType::RIGHT_SHIFT;
                    value = ">>";
                    goto RETURN_TOKEN;
                }
            }
            ttype = TokenType::GREATER;
            value = ">";
            break;
        case '<':
            current++;
            if (!is_at_end()) {
                if (*current == '=') {
                    current++;
                    ttype = TokenType::LESS_EQUAL;
                    value = "<=";
                    goto RETURN_TOKEN;
                } else if (*current == '<') {
                    current++;
                    ttype = TokenType::LEFT_SHIFT;
                    value = "<<";
                    goto RETURN_TOKEN;
                }
            }
            ttype = TokenType::LESS;
            value = "<";
            break;
        case '~':
            current++;
            ttype = TokenType::TILDE;
            value = "~";
            break;
        default: {
            Token tok;
            if (std::isdigit(*current))
                tok = generate_number_token();
            else if (*current == '"')
                tok = generate_string_token();
            else if (*current == '_' || std::isalpha(*current))
                tok = generate_identifier_token();
            else
                tok = generate_invalid_token();
            col += tok.value.length();
            return tok;
        }
    }
RETURN_TOKEN:
    col += value.length();
    return Token{ttype, value, col, lines.size()-1};
}
