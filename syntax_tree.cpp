#include <sstream>
#include "syntax_tree.hpp"

std::string Binary::to_string() const noexcept {
    std::ostringstream oss;
    oss << left->to_string() ;
    oss << ' ' << op.value << ' ' ;
    oss << right->to_string() ;
    return oss.str();
}

std::string Shift::to_string() const noexcept {
    std::ostringstream oss;
    oss << expr->to_string() ;
    oss << op.value ;
    oss << count->to_string() ;
    return oss.str();
}

Object* Shift::accept(Visitor* visitor) {
    return visitor->visit_shift(this);
}

Object* Comparison::accept(Visitor* visitor) {
    return visitor->visit_comparison(this);
}

std::string Term::to_string() const noexcept {
    std::ostringstream oss;
    oss << left_factor->to_string() ;
    oss << op.value ;
    oss << right_factor->to_string() ;
    return oss.str();
}

Object* Term::accept(Visitor* visitor) {
    return visitor->visit_term(this);
}

std::string Factor::to_string() const noexcept {
    std::ostringstream oss;
    oss << left_exponential->to_string() ;
    oss << op.value ;
    oss << right_exponential->to_string() ;
    return oss.str();
}

Object* Factor::accept(Visitor* visitor) {
    return visitor->visit_factor(this);
}

std::string Exponential::to_string() const noexcept {
    std::ostringstream oss;
    oss << base->to_string() ;
    oss << "**" ;
    oss << exponent->to_string() ;
    return oss.str();
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

