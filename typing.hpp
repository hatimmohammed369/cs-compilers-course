#ifndef TYPING_H_INCLUDED
#define TYPING_H_INCLUDED

#include "object.hpp"
#include "token.hpp"

class Type: public Object {
protected:
    Type() {type_info = type_type_object; }
public:
    std::string type_name{"type"};

    Object* copy() const noexcept override;
    ObjectBoolean* equals(const Object* other) const noexcept override;
    std::string to_string() const noexcept override;
    ObjectBoolean* to_boolean() const noexcept override;

    virtual Object* cast(const Object* obj) const noexcept;

    static Type* type_type_object;
    static Type* get_type_by_token(TokenType type_keyword);
};

class TypeInteger: public Type {
    TypeInteger(): Type() {
        this->type_name = "int";
    }
public:
    static TypeInteger* int_type_object;
    TypeInteger* copy() const noexcept override {
        return int_type_object;
    }
    ObjectInteger* cast(const Object* obj) const noexcept override;
};

class TypeFloat: public Type {
    TypeFloat(): Type() {
        this->type_name = "float";
    } 
public:
    static TypeFloat* float_type_object;
    TypeFloat* copy() const noexcept override {
        return float_type_object;
    }
    ObjectFloat* cast(const Object* obj) const noexcept override;
};

class TypeString: public Type {
    TypeString(): Type() {
        this->type_name = "string";
    } 
public:
    static TypeString* string_type_object;
    TypeString* copy() const noexcept override {
        return string_type_object;
    }
    ObjectString* cast(const Object* obj) const noexcept override;
};

class TypeBoolean: public Type {
    TypeBoolean(): Type() {
        this->type_name = "boolean";
    } 
public:
    static TypeBoolean* boolean_type_object;
    TypeBoolean* copy() const noexcept override {
        return boolean_type_object;
    }
    ObjectBoolean* cast(const Object* obj) const noexcept override;
};

class TypeVoid: public Type {
    TypeVoid(): Type() {
        this->type_name = "void";
    } 
public:
    static TypeVoid* void_type_object;
    TypeVoid* copy() const noexcept override {
        return void_type_object;
    }
    ObjectVoid* cast(const Object* obj) const noexcept override;
};

template <typename T>
ObjectBoolean* Number<T>::equals(const Object* other) const noexcept {
    if (
        other->type_info != TypeInteger::int_type_object &&
        other->type_info != TypeFloat::float_type_object
    )
        return ObjectBoolean::get_false_object();

    const ObjectInteger* other_int =
        dynamic_cast<const ObjectInteger*>(other);
    if (other_int)
        return ObjectBoolean::as_object(
            this->value == other_int->get()
        );

    const ObjectFloat* other_float =
        dynamic_cast<const ObjectFloat*>(other);
    return ObjectBoolean::as_object(
        this->value == other_float->get()
    );
}

#endif
