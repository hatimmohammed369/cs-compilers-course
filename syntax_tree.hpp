#ifndef SYNTAX_TREE_H_INCLUDED
#define SYNTAX_TREE_H_INCLUDED

#include "token.hpp"

class TreeBase {
public:
    ~TreeBase() {}
    virtual std::string to_string() const noexcept = 0;
};

inline std::ostream& operator<<(std::ostream& os, const TreeBase* tree_node) {
    return os << tree_node->to_string() ;
}

using expr_ptr = void*;

template <typename T>
class Expression: public TreeBase {
public:
    using type_ptr = T*;

    ~Expression() {}
    virtual expr_ptr evaluate() const noexcept = 0;
    type_ptr get_value() {
        return reinterpret_cast<type_ptr>(evaluate());
    }
};

class Void: public Expression<Void> {
private:
    Void() {} // Only a single object availaible
    Void(const Void&) = delete; // No copy constructor
    Void& operator=(const Void&) = delete; // No copy assignment
public:
    static Void* get_instance() {
        static Void void_object;
        return &void_object;
    }

    static std::string get_string_value() {
        static std::string value = std::string("void");
        return value;
    }

    std::string to_string() const noexcept override {
        return Void::get_string_value();
    }

    expr_ptr evaluate() const noexcept override {
        return reinterpret_cast<type_ptr>(Void::get_instance());
    }
};


template <typename T>
class Literal: public Expression<T> {
protected:
    T* value_object = new T;
public:
    Literal(const T& val) {*value_object = val;}
    ~Literal() {}

    expr_ptr evaluate() const noexcept override {
        return value_object;
    }
};

class String: public Literal<std::string> {
public:
    using Literal::Literal;

    std::string to_string() const noexcept override {
        std::string repr;
        repr.push_back('"');
        repr += *value_object;
        repr.push_back('"');
        return repr;
    }
};

class Boolean: public Literal<bool> {
public:
    using Literal::Literal;

    std::string to_string() const noexcept override {
        return std::string(*value_object ? "true" : "false");
    }
};

template <typename T>
class Number: public Literal<Token> {
public:
    using Literal::Literal;

    std::string to_string() const noexcept override {
        return value_object->value;
    }
};

class IntegerNumber: public Number<i64> {
public:
    using Number::Number;
    using Number::to_string;

    expr_ptr evaluate() const noexcept override {
        i64* value = new i64;
        *value = std::stol(this->value_object->value);
        return value;
    }
};

class FloatNumber: public Number<float64> {
public:
    using Number::Number;
    using Number::to_string;

    expr_ptr evaluate() const noexcept override {
        float64* value = new float64;
        *value = std::stold(this->value_object->value, nullptr);
        return value;
    }
};

#endif
