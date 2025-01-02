#include "lexer.hpp"
#include "token.hpp"
#include <cctype>

using namespace std;

void Lexer::init(char* in, size_t source_len) {
    this->current = this->source = in;
    this->source_length = source_len;
}

void Lexer::reset() {
    current = source;
}

bool Lexer::has_next() {
    ptrdiff_t diff = current - source;
    if (diff < 0) {
        cerr << "Error in lexer in line " << __LINE__ << "\n";
        cerr << "Current pointer before source pointer\n";
        exit(1);
    }
    return static_cast<size_t>(diff) < source_length;
}

Token Lexer::generate_number_token() {
    TokenType ttype;
    std::string value;
    // Generate a INTEGER/FLOAT token
    // Roughly it's: \d+([.]\d+((e|E)[+-]?\d+)?)?
    ttype = TOKEN_INTEGER;

    // Read digits before decimal point before end of input
    while (this->has_next() && isdigit(*current)) {
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

    if (!this->has_next() || !isdigit(*current)) {
        // Invalid numeric literal: no digits after decimal point
        ttype = TOKEN_INVALID;
        goto RETURN_TOKEN;
    }

    ttype = TOKEN_FLOAT;
    // Read digits after decimal point before end of input
    while (this->has_next() && isdigit(*current)) {
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

    if (!this->has_next() || !(isdigit(*current) || *current == '+' || *current == '-')) {
        // Invalid numeric literal: Missing exponent value
        ttype = TOKEN_INVALID;
        goto RETURN_TOKEN;
    }

    // Push exponent sign or first digit after e/E
    value.push_back(*current);
    // Move over
    current++;

    // Read digits after exponent sign
    while (this->has_next() && isdigit(*current)) {
        value.push_back(*current);
        current++;
    }

RETURN_TOKEN:
    return Token{ttype, value};
}

inline void Lexer::skip_whitespaces() {
    while (this->has_next() && isspace(*current))
        current++;
}

Token Lexer::generate_string_token() {
    TokenType ttype;
    std::string value;
    current++;
    ttype = TOKEN_STRING;
    while (this->has_next()) {
        current++;
        switch (*current) {
            case '"': goto RETURN_TOKEN;
            case '\\': {
                if (!this->has_next()) {
                    ttype = TOKEN_INVALID;
                    goto RETURN_TOKEN;
                }
                switch (*current) {
                    case '\\': {
                        // Quoted slash
                        value.push_back('\\');
                        break;
                    }
                    case '"': {
                        // Quoted "
                        value.push_back('"');
                        break;
                    }
                    case '0': {
                        // Null character
                        value.push_back('\0');
                        break;
                    }
                    case 'a': {
                        value.push_back('\a');
                        break;
                    }
                    case 'b': {
                        value.push_back('\b');
                        break;
                    }
                    case 'f': {
                        value.push_back('\f');
                        break;
                    }
                    case 'n': {
                        value.push_back('\n');
                        break;
                    }
                    case 'r': {
                        value.push_back('\r');
                        break;
                    }
                    case 't': {
                        value.push_back('\t');
                        break;
                    }
                    case 'v': {
                        value.push_back('\v');
                        break;
                    }
                    default: {
                        ttype = TOKEN_INVALID;
                        goto RETURN_TOKEN;
                    }
                }
            }
            default: value.push_back(*current);
        }
    }
RETURN_TOKEN:
    return Token{ttype, value};
}

Token Lexer::generate_identifier_token() {
    TokenType ttype = TOKEN_IDENTIFIER;
    std::string value;
    while (has_next() && std::isalnum(*current)) {
        value.push_back(*current);
        current++;
    }
    if (std::strcmp(value.c_str(), "and") == 0)
        ttype = TOKEN_KEYWORD_AND;
    else if (std::strcmp(value.c_str(), "bool") == 0)
        ttype = TOKEN_KEYWORD_BOOL;
    else if (std::strcmp(value.c_str(), "false") == 0)
        ttype = TOKEN_KEYWORD_FALSE;
    else if (std::strcmp(value.c_str(), "float") == 0)
        ttype = TOKEN_KEYWORD_FLOAT;
    else if (std::strcmp(value.c_str(), "int") == 0)
        ttype = TOKEN_KEYWORD_INT;
    else if (std::strcmp(value.c_str(), "or") == 0)
        ttype = TOKEN_KEYWORD_OR;
    else if (std::strcmp(value.c_str(), "string") == 0)
        ttype = TOKEN_KEYWORD_STRING;
    else if (std::strcmp(value.c_str(), "true") == 0)
        ttype = TOKEN_KEYWORD_TRUE;
    else if (std::strcmp(value.c_str(), "void") == 0)
        ttype = TOKEN_KEYWORD_VOID;
    else if (std::strcmp(value.c_str(), "xor") == 0)
        ttype = TOKEN_KEYWORD_XOR;
    return Token{ttype, value};
}

Token Lexer::get_next_token() {
    TokenType ttype;
    string value;
SKIP_WHITESPACES:
    skip_whitespaces();
    if (!has_next()) return Token{TOKEN_END_OF_FILE, string()};
    switch (*current) {
        case '{':
            current++;
            ttype = TOKEN_LEFT_CURLY_BRACE;
            value = "{";
            break;
        case '}':
            current++;
            ttype = TOKEN_RIGHT_CURLY_BRACE;
            value = "}";
            break;
        case '(':
            current++;
            ttype = TOKEN_LEFT_ROUND_BRACE;
            value = "(";
            break;
        case ')':
            current++;
            ttype = TOKEN_RIGHT_ROUND_BRACE;
            value = ")";
            break;
        case '!':
            current++;
            if (this->has_next() && *current == '=') {
                current++;
                ttype = TOKEN_LOGICAL_NOT_EQUAL;
                value = "!=";
            } else {
                ttype = TOKEN_BANG;
                value = "!";
            }
            break;
        case '=':
            current++;
            if (this->has_next() && *current == '=') {
                current++;
                ttype = TOKEN_LOGICAL_EQUAL;
                value = "==";
                break;
            } else {
                goto INVALID_TOKEN;
            }
        case '&':
            current++;
            ttype = TOKEN_BITWISE_AND;
            value = "&";
            break;
        case '|':
            current++;
            ttype = TOKEN_BITWISE_OR;
            value = "|";
            break;
        case '^':
            current++;
            ttype = TOKEN_BITWISE_XOR;
            value = "^";
            break;
        case '-':
            current++;
            ttype = TOKEN_MINUS;
            value = "-";
            break;
        case '*':
            current++;
            if (has_next() && *current == '*') {
                current++;
                ttype = TOKEN_EXPONENT;
                value = "**";
            } else {
                ttype = TOKEN_STAR;
                value = "*";
            }
            break;
        case '/':
            current++;
            if (has_next() && *current == '/') {
                current++;
                ttype = TOKEN_DOUBLE_SLASH;
                value = "//";
            } else {
                ttype = TOKEN_SLASH;
                value = "/";
            }
            break;
        case '%':
            current++;
            ttype = TOKEN_PERCENT;
            value = "%";
            break;
        case '+':
            current++;
            ttype = TOKEN_PLUS;
            value = "+";
            break;
        case '>':
            current++;
            if (has_next()) {
                if (*current == '=') {
                    current++;
                    ttype = TOKEN_GREATER_EQUAL;
                    value = ">=";
                    goto RETURN_TOKEN;
                } else if (*current == '>') {
                    current++;
                    ttype = TOKEN_RIGHT_SHIFT;
                    value = ">>";
                    goto RETURN_TOKEN;
                }
            }
            ttype = TOKEN_GREATER;
            value = ">";
            break;
        case '<':
            current++;
            if (has_next()) {
                if (*current == '=') {
                    current++;
                    ttype = TOKEN_LESS_EQUAL;
                    value = "<=";
                    goto RETURN_TOKEN;
                } else if (*current == '<') {
                    current++;
                    ttype = TOKEN_LEFT_SHIFT;
                    value = "<<";
                    goto RETURN_TOKEN;
                }
            }
            ttype = TOKEN_LESS;
            value = "<";
            break;
        case '~':
            current++;
            ttype = TOKEN_TILDE;
            value = "~";
            break;
        default: {
            if (isspace(*current))
                goto SKIP_WHITESPACES;
            else if (isdigit(*current))
                return generate_number_token();
            else if (*current == '"')
                return generate_string_token();
            else
                return generate_identifier_token();
        }
    }
INVALID_TOKEN:
    // Any other non-whitespace character
    ttype = TOKEN_INVALID;
    while (this->has_next() && !isspace(*current)) {
        value.push_back(*current);
        current++;
    }
    if (value.empty()) goto SKIP_WHITESPACES;
RETURN_TOKEN:
    return Token{ttype, value};
}
