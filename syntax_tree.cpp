#include "syntax_tree.hpp"

std::string Name::to_string() const noexcept {
    return name_str;
}

Object* Name::accept(Visitor* visitor) {
    return value_object;
}

std::string VariableDeclaration::to_string() const noexcept {
    std::ostringstream oss;
    oss << target_type->to_string() ;
    for (auto p : pairs) {
        oss << p.first ;
        if (p.second)
            oss << " = " << p.second->to_string() ;
        oss << ',' ;
    }
    oss << ';' ;
    return oss.str();
}

Object* VariableDeclaration::accept(Visitor* visitor) {
    return visitor->visit_variable_declaration(this);
}

std::string Cast::to_string() const noexcept {
    std::ostringstream oss;
    oss << '(' << this->target_type << ')' ;
    oss << casted_expr->to_string() ;
    return oss.str();
}

Object* Cast::accept(Visitor* visitor) {
    return visitor->visit_cast(this);
}

std::string Block::to_string() const noexcept {
    std::ostringstream oss;
    oss << "{" ;
    if (this->closing_newline)
        oss << '\n';
    for (
        auto stmt_ptr = this->statements.begin();
        stmt_ptr != this->statements.end()-1;
        stmt_ptr++
    ) {
        auto tree = *stmt_ptr;
        oss << tree->to_string() ;
        if (tree->end_token)
            oss << tree->end_token->value;
    }
    if (this->closing_newline)
        oss << '\n';
    oss << "}" ;
    return oss.str();
}

Object* Block::accept(Visitor* visitor) {
    return visitor->visit_block(this);
}

std::string Program::to_string() const noexcept {
    std::ostringstream oss;
    for (TreeBase* stmt : statements)
        oss << stmt->to_string() << '\n' ;
    return oss.str();
}

Object* Program::accept(Visitor* visitor) {
    return visitor->visit_program(this);
}

std::string Binary::to_string() const noexcept {
    std::ostringstream oss;
    oss << left->to_string() ;
    oss << ' ' << op.value << ' ' ;
    oss << right->to_string() ;
    return oss.str();
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
    std::ostringstream oss;
    oss << unary_op.value ;
    oss << expr->to_string() ;
    return oss.str();
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
    std::ostringstream oss;
    oss << '(' ;
    oss << grouped_expr->to_string() ;
    oss << ')' ;
    return oss.str();
}

Object* GroupedExpression::accept(Visitor* visitor) {
    return visitor->visit_grouped_expression(this);
}

