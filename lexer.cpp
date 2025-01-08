#include "lexer.hpp"
#include <cctype>

size_t Lexer::errors = 0;

void Lexer::init(char* in, const size_t& source_len) {
    source = in;
    current = source;
    current_line = source;
    current_line_length = 1;
    source_length = source_len;
}

void Lexer::reset() {
    current = source;
    line = 1;
    current_line = source;
    current_line_length = 1;
}

bool Lexer::has_next() {
    return (current + 1) < (source + source_length);
}

inline void Lexer::skip_whitespaces() {
    while (has_next() && std::isspace(*current)) {
        if (current > source && *(current-1) == '\n') {
            line++;
            computed_current_line_length = false;
            current_line_length = 1;
        }
        current++;
    }
    if (!computed_current_line_length) {
        computed_current_line_length = true;
        current_line = current;
        while (current_line > source && *current_line != '\n') {
            current_line--;
        }
        const char* end = source + source_length;
        for (char* p = current_line;p < end && *p != '\n';p++) {
            current_line_length++;
        }
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
    char* token_start = current;
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
        fmt << "Error in line " << line << ":\n" ;
        fmt << "Invalid numeric literal: no digits after decimal point\n";
        fmt << std::string(current_line, current_line_length) << '\n';
        char* p = current_line;
        for (;p < token_start ;p++)
            fmt << ' ' ;
        for (size_t i = 1;i <= value.length();i++, p++)
            fmt << '^' ;
        Lexer::report_lexing_error(read_fmt());
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
    TokenType ttype;
    std::string value;
    // Skip opening "
    current++;
    ttype = TokenType::STRING;
    if (!has_next()) {
        ttype = TokenType::INVALID;
        goto RETURN_TOKEN;
    }
    for (;this->has_next();current++) {
        if (*current == '"') {
            // Skip closing "
            current++;
            break;
        }
        if (*current == '\\') {
            // Move to escaped character
            current++;
            if (!this->has_next()) {
                ttype = TokenType::INVALID;
                goto RETURN_TOKEN;
            }
            if (*current == '\\') {
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
                ttype = TokenType::INVALID;
                goto RETURN_TOKEN;
            }
        } else {
            value.push_back(*current);
        }
    }
RETURN_TOKEN:
    return Token{ttype, value};
}

Token Lexer::generate_identifier_token() {
    TokenType ttype = TokenType::IDENTIFIER;
    std::string value;
    while (has_next() && std::isalnum(*current)) {
        value.push_back(*current);
        current++;
    }
    if (std::strcmp(value.c_str(), "and") == 0)
        ttype = TokenType::KEYWORD_AND;
    else if (std::strcmp(value.c_str(), "boolean") == 0)
        ttype = TokenType::KEYWORD_BOOLEAN;
    else if (std::strcmp(value.c_str(), "false") == 0)
        ttype = TokenType::KEYWORD_FALSE;
    else if (std::strcmp(value.c_str(), "float") == 0)
        ttype = TokenType::KEYWORD_FLOAT;
    else if (std::strcmp(value.c_str(), "int") == 0)
        ttype = TokenType::KEYWORD_INT;
    else if (std::strcmp(value.c_str(), "or") == 0)
        ttype = TokenType::KEYWORD_OR;
    else if (std::strcmp(value.c_str(), "string") == 0)
        ttype = TokenType::KEYWORD_STRING;
    else if (std::strcmp(value.c_str(), "true") == 0)
        ttype = TokenType::KEYWORD_TRUE;
    else if (std::strcmp(value.c_str(), "void") == 0)
        ttype = TokenType::KEYWORD_VOID;
    else if (std::strcmp(value.c_str(), "xor") == 0)
        ttype = TokenType::KEYWORD_XOR;
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

    if (!has_next()) {
        return Token{TokenType::END_OF_FILE, std::string()};
    } else if (old_line < line) {
        return Token{TokenType::LINEBREAK, std::string("\n")};
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
