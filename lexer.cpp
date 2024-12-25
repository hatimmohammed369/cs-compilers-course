#include "lexer.hpp"
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

Token* Lexer::get_next_token() {
    if (!has_next()) {
        Token* end_of_file = new Token {END_OF_FILE, string()};
        return end_of_file;
    }
    TokenType ttype;
    string value;
SKIP_SPACES:
    while (isspace(*current)) current++;
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
            if (isspace(*current)) goto SKIP_SPACES;
            else if (isdigit(*current)) {
                // Generate a NUMBER token
                // Roughly it's: \d+([.]\d+((e|E)[+-]\d+)?)?
                ttype = NUMBER;

                // Read digits before decimal point
                while (isdigit(*current)) {
                    value.push_back(*current);
                    current++;
                }

                if (*current != '.') {
                    // We found an integer
                    break;
                }

                // Read decimal point
                value.push_back('.');
                // Move over decimal point
                current++;

                if (!isdigit(*current)) {
                    // Invalid numeric literal: no digits after decimal point
                    value.clear();
                    return nullptr;
                }

                // Read digits after decimal point
                while (isdigit(*current)) {
                    value.push_back(*current);
                    current++;
                }

                if (*current == 'e' || *current == 'E') {
                    // Push e
                    value.push_back(*current);
                    // Move over e
                    current++;
                } else {
                    // No exponent
                    break;
                }

                if (*current == '+' || *current == '-') {
                    // Push exponent sign
                    value.push_back(*current);
                    // Move over exponent sign
                    current++;
                    if (!isdigit(*current)) {
                        // Invalid numeric literal: Missing exponent value
                        value.clear();
                        return nullptr;
                    }
                    // Read digits after exponent sign
                    if (isdigit(*current)) {
                        while (isdigit(*current)) {
                            value.push_back(*current);
                            current++;
                        }
                    }
                } else {
                    // Invalid numeric literal: Missing exponent value
                    value.clear();
                    return nullptr;
                }
            } else {
                // Any other character
                // Just ignore it for now
                current++;
                goto SKIP_SPACES;
            }
    }
    Token* read = new Token;
    read->ttype = ttype;
    read->value = std::move(value);
    return read;
}
