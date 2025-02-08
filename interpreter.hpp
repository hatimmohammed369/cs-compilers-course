#ifndef INTERPRETER_H_INCLUDED
#define INTERPRETER_H_INCLUDED

#include "environment.hpp"
#include "syntax_tree.hpp"

class Interpreter: public Visitor {
    Environment env{};
public:
    InterpreterResult interpret(TreeBase* tree);
    InterpreterResult visit_program(Program* tree);
    InterpreterResult visit_literal(Literal* tree);
    InterpreterResult visit_grouped_expression(GroupedExpression* tree);
    InterpreterResult visit_unary(Unary* tree);
    InterpreterResult visit_exponential(Exponential* tree);
    InterpreterResult visit_factor(Factor* tree);
    InterpreterResult visit_term(Term* tree);
    InterpreterResult visit_comparison(Comparison* tree);
    InterpreterResult visit_shift(Shift* tree);
    InterpreterResult visit_equality(Equality* tree);
    InterpreterResult visit_bitwise(Bitwise* tree);
    InterpreterResult visit_logical(Logical* tree);
    InterpreterResult visit_block(Block* tree);
    InterpreterResult visit_cast(Cast* tree);
    InterpreterResult visit_variable_declaration(VariableDeclaration* tree);
    InterpreterResult visit_print(Print* tree);
    InterpreterResult visit_return(Return* tree);
    InterpreterResult visit_name(Name* tree);
};

#endif
