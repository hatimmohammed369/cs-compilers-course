#ifndef INTERPRETER_H_INCLUDED
#define INTERPRETER_H_INCLUDED

#include "syntax_tree.hpp"

class Interpreter: public Visitor {
public:
    Object* interpret(TreeBase* tree);
    Object* visit_literal(Literal* literal);
    Object* visit_grouped_expression(GroupedExpression* group);
};

#endif
