#ifndef SYNTAX_TREE_H_INCLUDED
#define SYNTAX_TREE_H_INCLUDED

#include "token.hpp"
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

class Exponential: public Expression {
    TreeBase* base;
    Token op;
    TreeBase* exponent;
public:
    friend class Interpreter;
    Exponential(TreeBase* _base, Token _op, TreeBase* _exponent):
        base{_base}, op{_op}, exponent{_exponent} {}
    virtual std::string to_string() const noexcept override;
    virtual Object* accept(Visitor* visitor) override;
};

class Unary: public Expression {
    Token unary_op;
    TreeBase* expr;
public:
    friend class Interpreter;
    Unary(Token op, TreeBase* node):
        unary_op{op}, expr{node} {}
    virtual std::string to_string() const noexcept override;
    virtual Object* accept(Visitor* visitor) override;
};

class Literal: public Expression {
    Object* value_object;
public:
    friend class Interpreter;
    Literal(Object* val): value_object{val} {}
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
