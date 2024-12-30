#include "interpreter.hpp"

Object* Interpreter::interpret(TreeBase* tree) {
    return tree->accept(this);
}

Object* Interpreter::visit_literal(Literal* literal) {
    return literal->value_object;
}

Object* Interpreter::visit_logical(Logical* logical) {
    ObjectBoolean* left =
         reinterpret_cast<ObjectBoolean*>(logical->left->accept(this));
    bool value = (logical->bang ? !left->get() : left->get());
    return (value ? ObjectBoolean::get_true_object() : ObjectBoolean::get_false_object());
}

