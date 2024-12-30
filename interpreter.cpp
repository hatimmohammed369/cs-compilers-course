#include "interpreter.hpp"
#include "common.hpp"
#include "object.hpp"
#include <cmath>

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

Object* Interpreter::visit_exponential(Exponential* exponential) {
    ObjectInteger *int_base, *int_exponent;
    ObjectFloat *float_base, *float_exponent;
    int_base =
        dynamic_cast<ObjectInteger*>(exponential->base->accept(this));
    if (int_base) goto FIND_EXPONENT;
    float_base =
        dynamic_cast<ObjectFloat*>(exponential->base->accept(this));
    if (!float_base) {
        std::cerr << "Numeric operator ** used with non-numeric base\n" ;
        exit(1);
    }
FIND_EXPONENT:
    int_exponent =
        dynamic_cast<ObjectInteger*>(exponential->exponent->accept(this));
    if (int_exponent) goto EVALUATE;
    float_exponent =
        dynamic_cast<ObjectFloat*>(exponential->exponent->accept(this));
    if (!float_exponent) {
        std::cerr << "Numeric operator ** used with non-numeric exponent\n" ;
        exit(1);
    }
EVALUATE:
    if (int_base && int_exponent) {
        return new ObjectInteger{
            static_cast<i64>(std::powl(int_base->value, int_exponent->value))
        };
    } else if (int_base && float_exponent) {
        return new ObjectFloat{
            static_cast<float64>(std::pow(int_base->value, float_exponent->value))
        };
    } else if (float_base && int_base) {
        return new ObjectFloat{
            static_cast<float64>(std::pow(float_base->value, int_exponent->value))
        };
    }
    return new ObjectFloat{
        static_cast<float64>(std::pow(float_base->value, float_exponent->value))
    };
}

