#ifndef INTERPRETER_H_INCLUDED
#define INTERPRETER_H_INCLUDED

#include "environment.hpp"
#include "syntax_tree.hpp"

class Interpreter: public Visitor {
    Environment env{};
public:
    Object* interpret(TreeBase* tree);
    Object* visit_program(Program* tree);
    Object* visit_literal(Literal* tree);
    Object* visit_grouped_expression(GroupedExpression* tree);
    Object* visit_unary(Unary* tree);
    Object* visit_exponential(Exponential* tree);
    Object* visit_factor(Factor* tree);
    Object* visit_term(Term* tree);
    Object* visit_comparison(Comparison* tree);
    Object* visit_shift(Shift* tree);
    Object* visit_equality(Equality* tree);
    Object* visit_bitwise(Bitwise* tree);
    Object* visit_logical(Logical* tree);
    Object* visit_block(Block* tree);
    Object* visit_cast(Cast* tree);
    Object* visit_variable_declaration(VariableDeclaration* tree);
    Object* visit_print(Print* tree);
    Object* visit_return(Return* tree);
    Object* visit_name(Name* tree);
};

#endif
