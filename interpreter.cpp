#include "interpreter.hpp"

Object* Interpreter::interpret(TreeBase* tree) {
    return tree->accept(this);
}

Object* Interpreter::visit_literal(Literal* literal) {
    return literal->value_object;
}

Object* Interpreter::visit_grouped_expression(GroupedExpression* group) {
    return group->grouped_expr->accept(this);
}

Object* Interpreter::visit_unary(Unary* unary) {
    switch (unary->unary_op.ttype) {
        case TOKEN_BANG: {
            ObjectBoolean* obj =
                dynamic_cast<ObjectBoolean*>(unary->expr->accept(this));
            if (!obj) {
                std::cerr << "Unary logical operator ! applied to non-boolean" ;
                exit(1);
            }
            return (
                !obj->get() ?
                ObjectBoolean::get_true_object() :
                ObjectBoolean::get_false_object()
            );
        }
        default: {
            std::cerr << "Invalid unary operator " << unary->unary_op.value << '\n';
            exit(1);
        }
    }
    return nullptr;
}
