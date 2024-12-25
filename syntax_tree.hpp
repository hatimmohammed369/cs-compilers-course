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
class Expression: TreeBase {
public:
    using expression_type = T;
    ~Expression() {}
    virtual expression_type evaluate() const noexcept = 0;
};

#endif
