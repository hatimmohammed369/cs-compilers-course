#ifndef VISITOR_H_INCLUDED
#define VISITOR_H_INCLUDED

#include "result.hpp"

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
class Program;
class Block;
class Cast;
class VariableDeclaration;
class Name;
class Print;
class Return;
class Assignment;

using InterpreterResult =
    PointerValueResult<Object*, std::string>;

class Visitor {
public:
    ~Visitor() = default;
    virtual InterpreterResult visit_program(Program* tree) = 0;
    virtual InterpreterResult visit_literal(Literal* tree) = 0;
    virtual InterpreterResult visit_grouped_expression(GroupedExpression* tree) = 0;
    virtual InterpreterResult visit_unary(Unary* tree) = 0;
    virtual InterpreterResult visit_exponential(Exponential* tree) = 0;
    virtual InterpreterResult visit_factor(Factor* tree) = 0;
    virtual InterpreterResult visit_term(Term* tree) = 0;
    virtual InterpreterResult visit_comparison(Comparison* tree) = 0;
    virtual InterpreterResult visit_shift(Shift* tree) = 0;
    virtual InterpreterResult visit_equality(Equality* tree) = 0;
    virtual InterpreterResult visit_bitwise(Bitwise* tree) = 0;
    virtual InterpreterResult visit_logical(Logical* tree) = 0;
    virtual InterpreterResult visit_block(Block* tree) = 0;
    virtual InterpreterResult visit_cast(Cast* tree) = 0;
    virtual InterpreterResult visit_variable_declaration(VariableDeclaration* tree) = 0;
    virtual InterpreterResult visit_print(Print* tree) = 0;
    virtual InterpreterResult visit_return(Return* tree) = 0;
    virtual InterpreterResult visit_name(Name* tree) = 0;
    virtual InterpreterResult visit_assignment(Assignment* tree) = 0;
};

#endif
