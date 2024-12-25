#include "lexer.hpp"
#include "token.hpp"
#include <cctype>

using namespace std;

void Lexer::init(char* in, size_t source_len) {
    this->current = this->source = in;
    this->source_length = source_len;
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

Token Lexer::get_next_token() {
    TokenType ttype;
    string value;
SKIP_WHITESPACES:
    while (this->has_next() && isspace(*current)) current++;
    if (!has_next()) return Token{END_OF_FILE, string()};
    switch (*current) {
        case '{':
            ttype = LEFT_CURLY_BRACE;
            value = "{";
            // Next
            current++;
            break;
        case '}':
            ttype = RIGHT_CURLY_BRACE;
            value = "}";
            // Next
            current++;
            break;
        default:
            if (isspace(*current)) goto SKIP_WHITESPACES;
            else if (isdigit(*current)) {
                // Generate a NUMBER token
                // Roughly it's: \d+([.]\d+((e|E)[+-]?\d+)?)?
                ttype = NUMBER;

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
                    ttype = INVALID;
                    goto RETURN_TOKEN;
                }

                // Read digits after decimal point before end of input
                while (this->has_next() && isdigit(*current)) {
                    value.push_back(*current);
                    current++;
                }

                if (!this->has_next()) {
                    // No exponent
                    goto RETURN_TOKEN;
                } else if (*current == 'e' || *current == 'E') {
                    // Push e
                    value.push_back(*current);
                    // Move over e
                    current++;
                }

                if (!this->has_next()) {
                    // Invalid numeric literal: Missing exponent value
                    ttype = INVALID;
                    goto RETURN_TOKEN;
                } else if (*current == '+' || *current == '-') {
                    // Push exponent sign
                    value.push_back(*current);
                    // Move over exponent sign
                    current++;
                }

                if (!this->has_next() || !isdigit(*current)) {
                    // Invalid numeric literal: Missing exponent value
                    ttype = INVALID;
                    goto RETURN_TOKEN;
                }

                // Read digits after exponent sign
                while (this->has_next() && isdigit(*current)) {
                    value.push_back(*current);
                    current++;
                }
            } else {
                // Any other non-whitespace character
                ttype = INVALID;
                while (this->has_next() && !isspace(*current)) {
                    value.push_back(*current);
                    current++;
                }
                if (value.empty()) goto SKIP_WHITESPACES;
            }
    }
RETURN_TOKEN:
    return Token{ttype, value};
}
