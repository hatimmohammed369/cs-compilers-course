#include "syntax_tree.hpp"
#include <sstream>

Literal::Literal(Object* val): value_object{val} {}

std::string Literal::to_string() const noexcept {
    return value_object->to_string();
}

Object* Literal::accept(Visitor* visitor) {
    return visitor->visit_literal(this);
}

std::string Logical::to_string() const noexcept  {
    std::ostringstream oss;
    if (bang) oss << bang->value ;
    oss << left->to_string() ;
    oss << op.value ;
    oss << right->to_string() ;
    return oss.str() ;
}

Object* Logical::accept(Visitor* visitor)  {
    return visitor->visit_logical(this);
}
