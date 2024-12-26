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

template <typename T>
class Expression: public TreeBase {
public:
    using expr_ptr = void*;
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

    Expression<Void>::expr_ptr evaluate() const noexcept override {
        return reinterpret_cast<type_ptr>(Void::get_instance());
    }
};

template <typename T>
class Number: public Expression<T> {
protected:
    Token number;
public:
    Number(Token t): number{t} {}

    std::string to_string() const noexcept override {
        return number.value;
    }
};

class IntegerNumber: public Number<i64> {
public:
    using Number::Number;
    using Number::to_string;

    Expression::expr_ptr evaluate() const noexcept override {
        i64* value = new i64;
        *value = std::stol(number.value);
        return value;
    }
};

class FloatNumber: public Number<float64> {
public:
    using Number::Number;
    using Number::to_string;

    Expression::expr_ptr evaluate() const noexcept override {
        float64* value = new float64;
        *value = std::stold(number.value, nullptr);
        return value;
    }
};

#endif
