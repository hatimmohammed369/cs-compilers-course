#ifndef SYNTAX_TREE_H_INCLUDED
#define SYNTAX_TREE_H_INCLUDED

#include "object.hpp"
#include "visitor.hpp"

class TreeBase {
public:
    ~TreeBase() {}
    virtual std::string to_string() const noexcept = 0;
    virtual Object* accept(Visitor* visitor) = 0;
};

inline std::ostream& operator<<(std::ostream& os, const TreeBase* tree_node) {
    return os << tree_node->to_string() ;
}

class Expression: public TreeBase {
public:
    ~Expression() {}
};

class Literal: public Expression {
    Object* value_object;
public:
    friend class Interpreter;
    Literal(Object* val);
    virtual std::string to_string() const noexcept override;
    virtual Object* accept(Visitor* visitor) override;
};

class GroupedExpression: public Expression {
    TreeBase* grouped_expr;
public:
    friend class Interpreter;
    GroupedExpression(TreeBase* expr):
        grouped_expr{expr} {}
    virtual std::string to_string() const noexcept override;
    virtual Object* accept(Visitor* visitor) override;
};

#endif
