#include "interpreter.hpp"

Object* Interpreter::interpret(TreeBase* tree) {
    return tree->accept(this);
}

Object* Interpreter::visit_literal(Literal* literal) {
    return literal->value_object;
}
