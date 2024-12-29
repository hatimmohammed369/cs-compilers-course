#include "syntax_tree.hpp"

Literal::Literal(Object* val): value_object{val} {}

std::string Literal::to_string() const noexcept {
    return value_object->to_string();
}

Object* Literal::accept(Visitor* visitor) {
    return visitor->visit_literal(this);
}
