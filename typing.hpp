#ifndef TYPING_H_INCLUDED
#define TYPING_H_INCLUDED

#include "object.hpp"

enum TypeName {
    TYPENAME_TYPE,
    TYPENAME_INT,
    TYPENAME_FLOAT,
    TYPENAME_BOOL,
    TYPENAME_STRING,
    TYPENAME_VOID,
};

static const char* type_name_string(const TypeName& name) {
    switch (name) {
        case TYPENAME_TYPE:
            return "type";
        case TYPENAME_INT:
            return "int";
        case TYPENAME_FLOAT:
            return "float";
        case TYPENAME_BOOL:
            return "bool";
        case TYPENAME_STRING:
            return "string";
        case TYPENAME_VOID:
            return "void";
        default: {}
    }
    return "MISSING_TYPE";
}

class Type {
public:
    template <typename Target>
    Target* cast(const Target* obj);
};

#endif
