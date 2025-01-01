#ifndef VISITOR_H_INCLUDED
#define VISITOR_H_INCLUDED

class Object;
class Literal;
class GroupedExpression;
class Unary;
class Exponential;
class Factor;
class Term;
class Comparison;
class Shift;
class Equality;
class BitwiseAnd;
class BitwiseXor;
class BitwiseOr;
class LogicalAnd;
class LogicalOr;
class LogicalXor;

class Visitor {
public:
    ~Visitor() = default;
    virtual Object* visit_literal(Literal* literal) = 0;
    virtual Object* visit_grouped_expression(GroupedExpression* group) = 0;
    virtual Object* visit_unary(Unary* unary) = 0;
    virtual Object* visit_exponential(Exponential* exponential) = 0;
    virtual Object* visit_factor(Factor* factor) = 0;
    virtual Object* visit_term(Term* term) = 0;
    virtual Object* visit_comparison(Comparison* comparison) = 0;
    virtual Object* visit_shift(Shift* shift) = 0;
    virtual Object* visit_equality(Equality* equality) = 0;
    virtual Object* visit_bitwise_and(BitwiseAnd* bitwise_and) = 0;
    virtual Object* visit_bitwise_xor(BitwiseXor* bitwise_xor) = 0;
    virtual Object* visit_bitwise_or(BitwiseOr* bitwise_or) = 0;
    virtual Object* visit_logical_and(LogicalAnd* logical_and) = 0;
    virtual Object* visit_logical_or(LogicalOr* logical_or) = 0;
    virtual Object* visit_logical_xor(LogicalXor* logical_xor) = 0;
};

#endif
