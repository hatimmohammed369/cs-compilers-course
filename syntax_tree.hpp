#ifndef SYNTAX_TREE_H_INCLUDED
#define SYNTAX_TREE_H_INCLUDED

#include "token.hpp"

class TreeBase {
public:
    ~TreeBase() {}
    virtual std::string to_string() const noexcept = 0;
};

inline std::ostream& operator<<(std::ostream& os, const TreeBase& tree_node) {
    return os << tree_node.to_string() ;
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

class IntegerNumber: public Expression<i64> {
    Token number;
public:
    IntegerNumber(Token t): number{t} {}

    Expression::expr_ptr evaluate() const noexcept override {
        i64* value = new i64;
        *value = std::stol(number.value);
        return value;
    }

    std::string to_string() const noexcept {
        return number.value;
    }
};

#endif
