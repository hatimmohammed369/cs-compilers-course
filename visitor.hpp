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
class Bitwise;
class Logical;

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
    virtual Object* visit_bitwise(Bitwise* bitwise) = 0;
    virtual Object* visit_logical(Logical* logical) = 0;
};

#endif
