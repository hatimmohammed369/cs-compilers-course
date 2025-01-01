#include <sstream>
#include "syntax_tree.hpp"

std::string Binary::to_string() const noexcept {
    std::ostringstream oss;
    oss << left->to_string() ;
    oss << ' ' << op.value << ' ' ;
    oss << right->to_string() ;
    return oss.str();
}

Object* LogicalXor::accept(Visitor* visitor) {
    return visitor->visit_logical_xor(this);
}

Object* LogicalOr::accept(Visitor* visitor) {
    return visitor->visit_logical_or(this);
}

Object* LogicalAnd::accept(Visitor* visitor) {
    return visitor->visit_logical_and(this);
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

