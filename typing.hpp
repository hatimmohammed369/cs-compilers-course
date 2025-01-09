#ifndef TYPING_H_INCLUDED
#define TYPING_H_INCLUDED

#include "object.hpp"
#include "token.hpp"

class Type: public Object {
public:
    std::string type_name = NAME;
    const static std::string NAME;
    static inline Type* get_type_object() {
        static Type* type_type_object =
            new Type;
        type_type_object->type_info = type_type_object;
        return type_type_object;
    }
    Object* copy() const noexcept override;
    ObjectBoolean* equals(const Object* other) const noexcept override;
    std::string to_string() const noexcept override;
    ObjectBoolean* to_boolean() const noexcept override;

    virtual Object* cast(const Object* obj) const noexcept;

    static Type* get_type_by_token(TokenType type_keyword);
};

class TypeInteger: public Type {
    TypeInteger(): Type() {
        this->type_name = NAME;
    }
public:
    const static std::string NAME;
    static inline TypeInteger* get_type_object() {
        static TypeInteger* int_type_object =
            new TypeInteger;
        int_type_object->type_info = Type::get_type_object();
        return int_type_object;
    }
    TypeInteger* copy() const noexcept override {
        return get_type_object();
    }
    ObjectInteger* cast(const Object* obj) const noexcept override;
};

class TypeFloat: public Type {
    TypeFloat(): Type() {
        this->type_name = NAME;
    } 
public:
    const static std::string NAME;
    static inline TypeFloat* get_type_object() {
        static TypeFloat* float_type_object =
            new TypeFloat;
        float_type_object->type_info = Type::get_type_object();
        return float_type_object;
    }
    TypeFloat* copy() const noexcept override {
        return get_type_object();
    }
    ObjectFloat* cast(const Object* obj) const noexcept override;
};

class TypeString: public Type {
    TypeString(): Type() {
        this->type_name = NAME;
    } 
public:
    const static std::string NAME;
    static inline TypeString* get_type_object() {
        static TypeString* string_type_object =
            new TypeString;
        string_type_object->type_info = Type::get_type_object();
        return string_type_object;
    }
    TypeString* copy() const noexcept override {
        return get_type_object();
    }
    ObjectString* cast(const Object* obj) const noexcept override;
};

class TypeBoolean: public Type {
    TypeBoolean(): Type() {
        this->type_name = NAME;
    } 
public:
    const static std::string NAME;
    static inline TypeBoolean* get_type_object() {
        static TypeBoolean* boolean_type_object =
            new TypeBoolean;
        boolean_type_object->type_info = Type::get_type_object();
        return boolean_type_object;
    }
    TypeBoolean* copy() const noexcept override {
        return get_type_object();
    }
    ObjectBoolean* cast(const Object* obj) const noexcept override;
};

class TypeVoid: public Type {
    TypeVoid(): Type() {
        this->type_name = NAME;
    } 
public:
    const static std::string NAME;
    static inline TypeVoid* get_type_object() {
        static TypeVoid* void_type_object =
            new TypeVoid;
        void_type_object->type_info = Type::get_type_object();
        return void_type_object;
    }
    TypeVoid* copy() const noexcept override {
        return get_type_object();
    }
    ObjectVoid* cast(const Object* obj) const noexcept override;
};

template <typename T>
ObjectBoolean* Number<T>::equals(const Object* other) const noexcept {
    if (
        other->type_info != TypeInteger::get_type_object() &&
        other->type_info != TypeFloat::get_type_object()
    )
        return ObjectBoolean::FALSE;

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
