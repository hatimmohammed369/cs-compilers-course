#ifndef SYNTAX_TREE_H_INCLUDED
#define SYNTAX_TREE_H_INCLUDED

#include "object.hpp"
#include "token.hpp"
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
public:
    Object* value_object;
    Literal(Object* val);
    virtual std::string to_string() const noexcept override;
    virtual Object* accept(Visitor* visitor) override;
};

class Logical: public Expression {
    Token* bang;
    TreeBase* left;
    Token op;
    TreeBase* right;
public:
    friend class Interpreter;
    Logical(Token* bang_tok, TreeBase* lhs, Token op_tok, TreeBase* rhs):
        bang{bang_tok}, left{lhs}, op{op_tok}, right{rhs} {}
    virtual std::string to_string() const noexcept override;
    virtual Object* accept(Visitor* visitor) override;
};

#endif
