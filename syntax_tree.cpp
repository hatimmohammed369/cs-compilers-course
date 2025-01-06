#include "syntax_tree.hpp"

std::string Name::to_string() const noexcept {
    return name_str;
}

Object* Name::accept(Visitor* visitor) {
    return visitor->visit_name(this);
}

std::string VariableDeclaration::to_string() const noexcept {
    fmt << target_type->to_string() ;
    for (auto p : pairs) {
        fmt << p.first ;
        if (p.second)
            fmt << " = " << p.second->to_string() ;
        fmt << ',' ;
    }
    fmt << ';' ;
    std::string str = fmt.str();
    reset_fmt();
    return str;
}

Object* VariableDeclaration::accept(Visitor* visitor) {
    return visitor->visit_variable_declaration(this);
}

std::string Cast::to_string() const noexcept {
    fmt << '(' << this->target_type->to_string() << ')' ;
    fmt << casted_expr->to_string() ;
    std::string str = fmt.str();
    reset_fmt();
    return str;
}

Object* Cast::accept(Visitor* visitor) {
    return visitor->visit_cast(this);
}

std::string Block::to_string() const noexcept {
    fmt << "{" ;
    if (this->closing_newline)
        fmt << '\n';
    for (
        auto stmt_ptr = this->statements.begin();
        stmt_ptr != this->statements.end()-1;
        stmt_ptr++
    ) {
        auto tree = *stmt_ptr;
        fmt << tree->to_string() ;
        if (tree->end_token)
            fmt << tree->end_token->value;
    }
    if (this->closing_newline)
        fmt << '\n';
    fmt << "}" ;
    std::string str = fmt.str();
    reset_fmt();
    return str;
}

Object* Block::accept(Visitor* visitor) {
    return visitor->visit_block(this);
}

std::string Program::to_string() const noexcept {
    for (TreeBase* stmt : statements)
        fmt << stmt->to_string() << '\n' ;
    std::string str = fmt.str();
    reset_fmt();
    return str;
}

Object* Program::accept(Visitor* visitor) {
    return visitor->visit_program(this);
}

std::string Binary::to_string() const noexcept {
    fmt << left->to_string() ;
    fmt << ' ' << op.value << ' ' ;
    fmt << right->to_string() ;
    std::string str = fmt.str();
    reset_fmt();
    return str;
}

Object* Logical::accept(Visitor* visitor) {
    return visitor->visit_logical(this);
}

Object* Bitwise::accept(Visitor* visitor) {
    return visitor->visit_bitwise(this);
}

Object* Equality::accept(Visitor* visitor) {
    return visitor->visit_equality(this);
}

Object* Shift::accept(Visitor* visitor) {
    return visitor->visit_shift(this);
}

Object* Comparison::accept(Visitor* visitor) {
    return visitor->visit_comparison(this);
}

Object* Term::accept(Visitor* visitor) {
    return visitor->visit_term(this);
}

Object* Factor::accept(Visitor* visitor) {
    return visitor->visit_factor(this);
}

Object* Exponential::accept(Visitor* visitor) {
    return visitor->visit_exponential(this);
}

std::string Unary::to_string() const noexcept {
    fmt << unary_op.value ;
    fmt << expr->to_string() ;
    std::string str = fmt.str();
    reset_fmt();
    return str;
}

Object* Unary::accept(Visitor* visitor) {
    return visitor->visit_unary(this);
}

std::string Literal::to_string() const noexcept {
    return value_object->to_string();
}

Object* Literal::accept(Visitor* visitor) {
    return visitor->visit_literal(this);
}

std::string GroupedExpression::to_string() const noexcept {
    fmt << '(' ;
    fmt << grouped_expr->to_string() ;
    fmt << ')' ;
    std::string str = fmt.str();
    reset_fmt();
    return str;
}

Object* GroupedExpression::accept(Visitor* visitor) {
    return visitor->visit_grouped_expression(this);
}

