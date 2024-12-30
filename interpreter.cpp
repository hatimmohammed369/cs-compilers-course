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
        case TOKEN_MINUS: {
            Object* expr_value = unary->expr->accept(this);
            ObjectInteger* int_obj =
                dynamic_cast<ObjectInteger*>(expr_value);
            if (int_obj) return -(*int_obj);
            ObjectFloat* float_obj =
                dynamic_cast<ObjectFloat*>(expr_value);
            if (float_obj) return -(*float_obj);
            std::cerr << "Unary logical operator - applied to non-numeric" ;
            exit(1);
            break;
        }
        default: {
            std::cerr << "Invalid unary operator " << unary->unary_op.value << '\n';
            exit(1);
        }
    }
    return nullptr;
}
