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
    virtual Object* cast(const Object* obj) const noexcept = 0;
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
    TypeInt* copy() const noexcept override {
        return get_int_type_object();
    }
    ObjectInteger* cast(const Object* obj) const noexcept override;
};

class TypeFloat: public Type {
    TypeFloat() {name = TYPENAME_FLOAT;}
public:
    static TypeFloat* get_float_type_object() {
        static TypeFloat* float_type_obj =
            new TypeFloat;
        return float_type_obj;
    }
    TypeFloat* copy() const noexcept override {
        return get_float_type_object();
    }
    ObjectFloat* cast(const Object* obj) const noexcept override;
};

class TypeString: public Type {
    TypeString() {name = TYPENAME_STRING;}
public:
    static TypeString* get_string_type_object() {
        static TypeString* string_type_obj =
            new TypeString;
        return string_type_obj;
    }
    TypeString* copy() const noexcept override {
        return get_string_type_object();
    }
};

class TypeBoolean: public Type {
    TypeBoolean() {name = TYPENAME_BOOLEAN;}
public:
    static TypeBoolean* get_boolean_type_object() {
        static TypeBoolean* boolean_type_obj =
            new TypeBoolean;
        return boolean_type_obj;
    }
    TypeBoolean* copy() const noexcept override {
        return get_boolean_type_object();
    }
};

class TypeVoid: public Type {
    TypeVoid() {name = TYPENAME_VOID;}
public:
    static TypeVoid* get_void_type_object() {
        static TypeVoid* void_type_obj =
            new TypeVoid;
        return void_type_obj;
    }
    TypeVoid* copy() const noexcept override {
        return get_void_type_object();
    }
};

#endif
