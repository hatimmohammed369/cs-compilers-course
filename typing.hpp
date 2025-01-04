#ifndef TYPING_H_INCLUDED
#define TYPING_H_INCLUDED

#include "object.hpp"
#include "token.hpp"

enum TypeName {
    TYPENAME_TYPE,
    TYPENAME_INT,
    TYPENAME_FLOAT,
    TYPENAME_BOOLEAN,
    TYPENAME_STRING,
    TYPENAME_VOID,
};

class Type: public Object {
protected:
    Type() {tag = OBJECT_TYPE;}
public:
    TypeName name;
    virtual ~Type() = default;
    std::string to_string() const noexcept;
    ObjectBoolean* to_boolean() const noexcept;
    ObjectBoolean* equals(const Object* other) const noexcept;
    static Type* get_type_by_token(TokenType type_keyword);
};

class TypeInt: public Type {
    TypeInt() {name = TYPENAME_INT;}
public:
    static TypeInt* get_int_type_object() {
        static TypeInt* int_type_obj =
            new TypeInt;
        return int_type_obj;
    }
};

class TypeFloat: public Type {
    TypeFloat() {name = TYPENAME_FLOAT;}
public:
    static TypeFloat* get_int_type_object() {
        static TypeFloat* int_type_obj =
            new TypeFloat;
        return int_type_obj;
    }
};

class TypeString: public Type {
    TypeString() {name = TYPENAME_STRING;}
public:
    static TypeString* get_int_type_object() {
        static TypeString* int_type_obj =
            new TypeString;
        return int_type_obj;
    }
};

#endif
