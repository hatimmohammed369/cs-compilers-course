#ifndef INTERPRETER_H_INCLUDED
#define INTERPRETER_H_INCLUDED

#include "syntax_tree.hpp"

class Interpreter: public Visitor {
public:
    Object* interpret(TreeBase* tree);
    Object* visit_literal(Literal* literal);
    Object* visit_grouped_expression(GroupedExpression* group);
    Object* visit_unary(Unary* unary);
    Object* visit_exponential(Exponential* exponential);
    Object* visit_factor(Factor* factor);
    Object* visit_term(Term* term);
    Object* visit_comparison(Comparison* comparison);
    Object* visit_shift(Shift* shift);
    Object* visit_equality(Equality* equality);
    Object* visit_bitwise(Bitwise* bitwise);
    Object* visit_logical(Logical* logical);
};

#endif
