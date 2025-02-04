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
class Program;
class Block;
class Cast;
class VariableDeclaration;
class Name;
class Print;
class Return;

class Visitor {
public:
    ~Visitor() = default;
    virtual Object* visit_program(Program* tree) = 0;
    virtual Object* visit_literal(Literal* tree) = 0;
    virtual Object* visit_grouped_expression(GroupedExpression* tree) = 0;
    virtual Object* visit_unary(Unary* tree) = 0;
    virtual Object* visit_exponential(Exponential* tree) = 0;
    virtual Object* visit_factor(Factor* tree) = 0;
    virtual Object* visit_term(Term* tree) = 0;
    virtual Object* visit_comparison(Comparison* tree) = 0;
    virtual Object* visit_shift(Shift* tree) = 0;
    virtual Object* visit_equality(Equality* tree) = 0;
    virtual Object* visit_bitwise(Bitwise* tree) = 0;
    virtual Object* visit_logical(Logical* tree) = 0;
    virtual Object* visit_block(Block* tree) = 0;
    virtual Object* visit_cast(Cast* tree) = 0;
    virtual Object* visit_variable_declaration(VariableDeclaration* tree) = 0;
    virtual Object* visit_print(Print* tree) = 0;
    virtual Object* visit_return(Return* tree) = 0;
    virtual Object* visit_name(Name* tree) = 0;
};

#endif
