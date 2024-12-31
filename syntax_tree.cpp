#include "syntax_tree.hpp"
#include <sstream>

std::string Comparison::to_string() const noexcept {
    std::ostringstream oss;
    oss << left_term->to_string() ;
    oss << op.value ;
    oss << right_term->to_string() ;
    return oss.str();
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

