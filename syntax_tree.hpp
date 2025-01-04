#ifndef SYNTAX_TREE_H_INCLUDED
#define SYNTAX_TREE_H_INCLUDED

#include <vector>
#include "token.hpp"
#include "object.hpp"
#include "typing.hpp"
#include "visitor.hpp"

class TreeBase {
public:
    ~TreeBase() = default;
    virtual std::string to_string() const noexcept = 0;
    virtual Object* accept(Visitor* visitor) = 0;
};

inline std::ostream& operator<<(std::ostream& os, const TreeBase* tree_node) {
    return os << tree_node->to_string() ;
}

class Program: public TreeBase {
public:
    std::vector<TreeBase*> statements;
    std::string to_string() const noexcept override;
    Object* accept(Visitor* visitor) override;
};

class Statement: public TreeBase {
public:
    Token* end_token = nullptr;
    ~Statement() = default;
};

class Expression: public Statement {
public:
    ~Expression() = default;
};

class Cast: public Expression {
public:
    Type* target_type;
    TreeBase* casted_expr;
    Cast(Type* to_type, TreeBase* expr):
        target_type{to_type}, casted_expr{expr} {}
    std::string to_string() const noexcept override;
    Object* accept(Visitor* visitor) override;
};

class Block: public Expression {
public:
    std::vector<Statement*> statements;
    Token* opening_newline = nullptr;
    Token* closing_newline = nullptr;
    std::string to_string() const noexcept override;
    Object* accept(Visitor* visitor) override;
};

class Binary: public Expression {
public:
    TreeBase* left;
    Token op;
    TreeBase* right;
    Binary(TreeBase* lhs, Token _op, TreeBase* rhs):
        left{lhs}, op{_op}, right{rhs} {}
    std::string to_string() const noexcept override;
};

class Logical: public Binary {
public:
    using Binary::Binary;
    Object* accept(Visitor* visitor) override;
};

class Bitwise: public Binary {
public:
    using Binary::Binary;
    Object* accept(Visitor* visitor) override;
};

class Equality: public Binary {
public:
    using Binary::Binary;
    Object* accept(Visitor* visitor) override;
};

class Comparison: public Binary {
public:
    using Binary::Binary;
    Object* accept(Visitor* visitor) override;
};

class Shift: public Binary {
public:
    using Binary::Binary;
    Object* accept(Visitor* visitor) override;
};

class Term: public Binary {
public:
    using Binary::Binary;
    Object* accept(Visitor* visitor) override;
};

class Factor: public Binary {
public:
    using Binary::Binary;
    Object* accept(Visitor* visitor) override;
};

class Exponential: public Binary {
public:
    using Binary::Binary;
    Object* accept(Visitor* visitor) override;
};

class Unary: public Expression {
public:
    Token unary_op;
    TreeBase* expr;
    Unary(Token op, TreeBase* node):
        unary_op{op}, expr{node} {}
    virtual std::string to_string() const noexcept override;
    virtual Object* accept(Visitor* visitor) override;
};

class Literal: public Expression {
public:
    Object* value_object;
    Literal(Object* val): value_object{val} {}
    virtual std::string to_string() const noexcept override;
    virtual Object* accept(Visitor* visitor) override;
};

class GroupedExpression: public Expression {
public:
    TreeBase* grouped_expr;
    GroupedExpression(TreeBase* expr):
        grouped_expr{expr} {}
    virtual std::string to_string() const noexcept override;
    virtual Object* accept(Visitor* visitor) override;
};

#endif
