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
                // Generate a INTEGER/FLOAT token
                // Roughly it's: \d+([.]\d+((e|E)[+-]?\d+)?)?
                ttype = INTEGER;

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

                ttype = FLOAT;
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
                    ttype = INVALID;
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
            } else {
                if (*current == '"') {
                    current++;
                    ttype = STRING;
                    while (this->has_next()) {
                        if (*current == '\\') {
                            current++;
                            if (!this->has_next()) {
                                ttype = INVALID;
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
                                    ttype = INVALID;
                                    goto RETURN_TOKEN;
                                }
                            }
                        } else {
                            if (*current == '"') {
                                current++;
                                goto RETURN_TOKEN;
                            }
                            value.push_back(*current);
                            current++;
                        }
                    }
                } else if (*current == 'v') {
                    // checking for 'void'
                    char* old = current;
                    current++;

                    if (*current == 'o') current++;
                    else {
                        current = old;
                        goto INVALID_TOKEN;
                    }

                    if (*current == 'i') current++;
                    else {
                        current = old;
                        goto INVALID_TOKEN;
                    }

                    if (*current == 'd') {
                        current++;
                        ttype = VOID;
                        value.clear();
                        value.append("void");
                        goto RETURN_TOKEN;
                    } else {
                        current = old;
                        goto INVALID_TOKEN;
                    }
                } else if (*current == 't') {
                    // checking for 'true'
                    char* old = current;
                    current++;

                    if (*current == 'r') current++;
                    else {
                        current = old;
                        goto INVALID_TOKEN;
                    }

                    if (*current == 'u') current++;
                    else {
                        current = old;
                        goto INVALID_TOKEN;
                    }

                    if (*current == 'e') {
                        current++;
                        ttype = TRUE;
                        value.clear();
                        value.append("true");
                        goto RETURN_TOKEN;
                    } else {
                        current = old;
                        goto INVALID_TOKEN;
                    }
                } else if (*current == 'f') {
                    // checking for 'false'
                    char* old = current;
                    current++;

                    if (*current == 'a') current++;
                    else {
                        current = old;
                        goto INVALID_TOKEN;
                    }

                    if (*current == 'l') current++;
                    else {
                        current = old;
                        goto INVALID_TOKEN;
                    }

                    if (*current == 's') current++;
                    else {
                        current = old;
                        goto INVALID_TOKEN;
                    }

                    if (*current == 'e') {
                        current++;
                        ttype = FALSE;
                        value.clear();
                        value.append("false");
                        goto RETURN_TOKEN;
                    } else {
                        current = old;
                        goto INVALID_TOKEN;
                    }
                }
            INVALID_TOKEN:
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
