#ifndef TYPING_H_INCLUDED
#define TYPING_H_INCLUDED

#include "object.hpp"
#include "token.hpp"

class Type: public Object {
protected:
    Type() {tag = OBJECT_TYPE;}
public:
    virtual ~Type() = default;
    std::string to_string() const noexcept;
    ObjectBoolean* to_boolean() const noexcept;
    ObjectBoolean* equals(const Object* other) const noexcept;
    virtual Object* cast(const Object* obj) const noexcept = 0;
    static Type* get_type_by_token(TokenType type_keyword);
};

class TypeInt: public Type {
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
public:
    static TypeString* get_string_type_object() {
        static TypeString* string_type_obj =
            new TypeString;
        return string_type_obj;
    }
    TypeString* copy() const noexcept override {
        return get_string_type_object();
    }
    ObjectString* cast(const Object* obj) const noexcept override;
};

class TypeBoolean: public Type {
public:
    static TypeBoolean* get_boolean_type_object() {
        static TypeBoolean* boolean_type_obj =
            new TypeBoolean;
        return boolean_type_obj;
    }
    TypeBoolean* copy() const noexcept override {
        return get_boolean_type_object();
    }
    ObjectBoolean* cast(const Object* obj) const noexcept override;
};

class TypeVoid: public Type {
public:
    static TypeVoid* get_void_type_object() {
        static TypeVoid* void_type_obj =
            new TypeVoid;
        return void_type_obj;
    }
    TypeVoid* copy() const noexcept override {
        return get_void_type_object();
    }
    ObjectVoid* cast(const Object* obj) const noexcept override;
};

#endif
