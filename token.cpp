#include "token.hpp"

const char* token_type_name(const TokenType& ttype) {
    switch (ttype) {
        case -1:
            return "INVALID" ;
        case 0:
            return "END_OF_FILE" ;
        case 1:
            return "INTEGER" ;
        case 2:
            return "FLOAT" ;
        case 3:
            return "LEFT_CURLY_BRACE" ;
        case 4:
            return "RIGHT_CURLY_BRACE" ;
        case 5:
            return "VOID" ;
        case 6:
            return "TRUE" ;
        case 7:
            return "FALSE" ;
        case 8:
            return "STRING" ;
        case 9:
            return "LEFT_ROUND_BRACE" ;
        case 10:
            return "RIGHT_ROUND_BRACE" ;
        case 11:
            return "BANG" ;
        case 12:
            return "LOGICAL_EQUAL"; 
        case 13:
            return "LOGICAL_NOT_EQUAL"; 
        case 14:
            return "LOGICAL_AND"; 
        case 15:
            return "LOGICAL_OR"; 
    }
    return "MISSING_CATEGORY" ;
}

inline std::ostream& operator<<(std::ostream& os, const TokenType& ttype) {
    return os << std::string(token_type_name(ttype)) ;
}

inline std::ostream& operator<<(std::ostream& os, const Token& token) {
    return os << "Token{ value='" << token.value << "', ttype=" << token.ttype << " }";
}
