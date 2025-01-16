#include <cctype>
#include <vector>
#include "lexer.hpp"

size_t Lexer::errors = 0;

void Lexer::init(char* in, const size_t& source_len) {
    source.assign(in, source_len);
    if (source.back() != '\n')
        source.push_back('\n');
    current = source.begin();
    line = 1;
    current_line.clear();
}

void Lexer::reset() {
    current = source.begin();
    line = 1;
    current_line.clear();
}

inline bool Lexer::has_next() {
    return current != source.end();
}

inline void Lexer::skip_whitespaces() {
    while (has_next() && std::isspace(*current)) {
        if (current != source.begin() && *(current-1) == '\n') {
            line++;
            current_line.clear();
        }
        current++;
    }
    if (!has_next()) {
        std::string::size_type last_line_break_in_input =
            source.rfind('\n');
        current_line.assign(
            source.substr(last_line_break_in_input)
        );
    } else if (current_line.empty()) {
        std::string::size_type cur_pos =
            std::distance(source.cbegin(), current);
        std::string::size_type last_line_break =
            source.find_last_of('\n', cur_pos);
        if (last_line_break == std::string::npos)
            last_line_break = 0;
        std::string::size_type next_line_break =
            source.find_first_of('\n', cur_pos);
        current_line.assign(
            source.substr(last_line_break, next_line_break)
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
    while (this->has_next() && std::isdigit(*current)) {
        value.push_back(*current);
        current++;
    }

    if (!this->has_next() || *current != '.') {
        // We found an integer
        goto RETURN_TOKEN;
    }

    // Read decimal point
    value.push_back('.');
    // Move over decimal point
    current++;

    if (!this->has_next() || !std::isdigit(*current)) {
        // Invalid numeric literal: no digits after decimal point
        fmt << std::string(std::distance(source.cbegin(), token_start), ' ') ;
        fmt << std::string(value.size(), '^') ;
        Lexer::report_lexing_error(
            "Invalid numeric literal: no digits after decimal point",
            read_fmt()
        );
        ttype = TokenType::INVALID;
        goto RETURN_TOKEN;
    }

    ttype = TokenType::FLOAT;
    // Read digits after decimal point before end of input
    while (this->has_next() && std::isdigit(*current)) {
        value.push_back(*current);
        current++;
    }

    if (!this->has_next() || !(*current == 'e' || *current == 'E')) {
        // No exponent
        goto RETURN_TOKEN;
    }

    // Push e
    value.push_back(*current);
    // Move over e
    current++;

    if (!this->has_next() || !(std::isdigit(*current) || *current == '+' || *current == '-')) {
        // Invalid numeric literal: Missing exponent value
        fmt << std::string(std::distance(source.cbegin(), token_start), ' ') ;
        fmt << std::string(value.size(), '^') ;
        Lexer::report_lexing_error(
            "Invalid numeric literal: Missing exponent value",
            read_fmt()
        );
        ttype = TokenType::INVALID;
        goto RETURN_TOKEN;
    }

    // Push exponent sign or first digit after e/E
    value.push_back(*current);
    // Move over
    current++;

    // Read digits after exponent sign
    while (this->has_next() && std::isdigit(*current)) {
        value.push_back(*current);
        current++;
    }

RETURN_TOKEN:
    return Token{ttype, value};
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
        if (!has_next()) {
            break;
        } else if (*current == '"') {
            // Skip closing "
            current++;
            terminated = true;
            break;
        } else if (*current == '\\') {
            // Move to escaped character
            current++;
            if (!this->has_next()) {
                break;
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
        ttype = TokenType::INVALID;
        size_t i = 0;
        for (auto pos : invalid_escapes) {
            while (i < pos) {
                fmt << ' ' ;
                i++;
            }
            fmt << "^^" ;
            i = pos + 2;
        }
        Lexer::report_lexing_error(
            "Invalid escape sequence",
            read_fmt()
        );
    }
    if (!terminated) {
        ttype = TokenType::INVALID;
        fmt << std::string(std::distance(source.cbegin(), token_start), ' ') ;
        fmt << std::string(value.size(), '^') ;
        Lexer::report_lexing_error(
            "Unterminated string literal",
            read_fmt()
        );
    }
    return Token{ttype, value};
}

Token Lexer::generate_identifier_token() {
    TokenType ttype = TokenType::IDENTIFIER;
    std::string value;
    while (has_next() && (*current == '_' || std::isalpha(*current))) {
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
    else if (value == "println")
        ttype = TokenType::KEYWORD_PRINTLN;
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
    return Token{ttype, value};
}

Token Lexer::generate_invalid_token() {
    // Any other non-whitespace character
    const TokenType ttype = TokenType::INVALID;
    std::string value;
    while (
        this->has_next() &&
        !is_valid_first_char(*current)
    ) {
        value.push_back(*current);
        current++;
    }
    return Token{ttype, value};
}

Token Lexer::generate_next_token() {
    size_t old_line = line;
    TokenType ttype;
    std::string value;
    skip_whitespaces();
    if (!has_next())
        return Token{TokenType::END_OF_FILE, std::string()};
    else if (old_line < line)
        return Token{TokenType::LINEBREAK, std::string("\n")};
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
            if (has_next() && *current == '=') {
                current++;
                ttype = TokenType::COLON_EQUAL;
                value = ":=";
                break;
            } else {
                current--;
                return generate_invalid_token();
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
            if (this->has_next() && *current == '=') {
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
            if (this->has_next() && *current == '=') {
                current++;
                ttype = TokenType::LOGICAL_EQUAL;
                value = "==";
                break;
            } else {
                current--;
                return generate_invalid_token();
            }
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
            if (has_next() && *current == '*') {
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
            if (has_next() && *current == '/') {
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
            if (has_next()) {
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
            if (has_next()) {
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
            return tok;
        }
    }
RETURN_TOKEN:
    return Token{ttype, value};
}
