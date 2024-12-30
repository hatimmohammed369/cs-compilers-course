#ifndef VISITOR_H_INCLUDED
#define VISITOR_H_INCLUDED

class Object;
class Literal;
class GroupedExpression;
class Unary;

class Visitor {
public:
    ~Visitor() = default;
    virtual Object* visit_literal(Literal* literal) = 0;
    virtual Object* visit_grouped_expression(GroupedExpression* group) = 0;
    virtual Object* visit_unary(Unary* unary) = 0;
};

#endif
