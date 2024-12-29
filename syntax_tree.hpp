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

class Expression: public TreeBase {
public:
    ~Expression() {}
};

class Void: public Expression {
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
    std::string to_string() const noexcept override;
};

template <typename T>
class Literal: public Expression {
    T* value_object = new T;
public:
    Literal(const T& val);
    virtual std::string to_string() const noexcept override;
};

template <>
std::string Literal<bool>::to_string() const noexcept;

template <>
std::string Literal<std::string>::to_string() const noexcept;

#endif
