#include <cmath>
#include "interpreter.hpp"
#include "common.hpp"
#include "object.hpp"
#include "token.hpp"

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
            return obj->negated();
        }
        case TOKEN_MINUS: {
            Object* expr_value = unary->expr->accept(this);
            ObjectInteger* int_obj =
                dynamic_cast<ObjectInteger*>(expr_value);
            if (int_obj)
                return -(*int_obj);

            ObjectFloat* float_obj =
                dynamic_cast<ObjectFloat*>(expr_value);
            if (float_obj)
                return -(*float_obj);

            std::cerr << "Unary logical operator - applied to non-numeric" ;
            exit(1);
        }
        case TOKEN_PLUS: {
            Object* expr_value = unary->expr->accept(this);
            ObjectInteger* int_obj =
                dynamic_cast<ObjectInteger*>(expr_value);
            if (int_obj)
                return int_obj;

            ObjectFloat* float_obj =
                dynamic_cast<ObjectFloat*>(expr_value);
            if (float_obj)
                return float_obj;

            std::cerr << "Unary logical operator + applied to non-numeric" ;
            exit(1);
        }
        case TOKEN_TILDE: {
            ObjectInteger* int_obj =
                dynamic_cast<ObjectInteger*>(unary->expr->accept(this));
            if (!int_obj) {
                std::cerr << "Unary logical operator + applied to non-numeric" ;
                exit(1);
            }
            return ~(*int_obj);
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
        dynamic_cast<ObjectInteger*>(exponential->left->accept(this));
    if (int_base) goto FIND_EXPONENT;
    float_base =
        dynamic_cast<ObjectFloat*>(exponential->left->accept(this));
    if (!float_base) {
        std::cerr << "Numeric operator ** used with non-numeric base\n" ;
        exit(1);
    }
FIND_EXPONENT:
    int_exponent =
        dynamic_cast<ObjectInteger*>(exponential->right->accept(this));
    if (int_exponent) goto EVALUATE;
    float_exponent =
        dynamic_cast<ObjectFloat*>(exponential->right->accept(this));
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
    } else if (float_base && int_exponent) {
        return new ObjectFloat{
            static_cast<float64>(std::pow(float_base->value, int_exponent->value))
        };
    }
    // float_base && float_exponent
    return new ObjectFloat{
        static_cast<float64>(std::pow(float_base->value, float_exponent->value))
    };
}

Object* Interpreter::visit_factor(Factor* factor) {
    ObjectInteger *left_int, *right_int;
    ObjectFloat *left_float, *right_float;
    left_int =
        dynamic_cast<ObjectInteger*>(
            factor->left->accept(this)
        );
    if (left_int) goto FIND_RIGHT;
    left_float =
        dynamic_cast<ObjectFloat*>(
            factor->left->accept(this)
        );
    if (!left_float) {
        std::cerr <<
            "Left operand of operator " <<
            factor->op.value <<
            " is not numeric \n";
        exit(1);
    }
FIND_RIGHT:
    right_int =
        dynamic_cast<ObjectInteger*>(
            factor->right->accept(this)
        );
    if (right_int) goto EVALUATE;
    right_float =
        dynamic_cast<ObjectFloat*>(
            factor->right->accept(this)
        );
    if (!right_float) {
        std::cerr <<
            "right operand of operator " <<
            factor->op.value <<
            " is not numeric \n";
        exit(1);
    }
EVALUATE:
    switch (factor->op.ttype) {
        case TOKEN_STAR: {
            if (left_int && right_int)
                return (*left_int) * right_int;
            else if (left_int && right_float)
                return (*left_int) * right_float;
            else if (left_float && right_int)
                return (*left_float) * right_int;
            return (*left_float) * right_float;
        }
        case TOKEN_SLASH: {
            if (left_int && right_int)
                return (*left_int) / right_int;
            else if (left_int && right_float)
                return (*left_int) / right_float;
            else if (left_float && right_int)
                return (*left_float) / right_int;
            return (*left_float) / right_float;
        }
        case TOKEN_DOUBLE_SLASH: {
            if (left_int && right_int)
                return left_int->integer_div(right_int);
            else if (left_int && right_float)
                return left_int->integer_div(right_float);
            else if (left_float && right_int)
                return left_float->integer_div(right_int);
            return left_float->integer_div(right_float);
        }
        case TOKEN_PERCENT: {
            if (!(left_int && right_int)) {
                std::cerr << "Applying mod operator % with a non-integer operand\n";
                exit(1);
            }
            return (*left_int) % right_int;
        }
        default: {
            std::cerr << "Invalid binary operator " << factor->op.value << " for numeric operands\n";
            exit(1);
        }
    }
    return nullptr;
}

Object* Interpreter::visit_term(Term* term) {
    ObjectInteger *left_int, *right_int;
    ObjectFloat *left_float, *right_float;
    left_int =
        dynamic_cast<ObjectInteger*>(
            term->left->accept(this)
        );
    if (left_int) goto FIND_RIGHT;
    left_float =
        dynamic_cast<ObjectFloat*>(
            term->left->accept(this)
        );
    if (!left_float) {
        std::cerr <<
            "Left operand of operator " <<
            term->op.value <<
            " is not numeric \n";
        exit(1);
    }
FIND_RIGHT:
    right_int =
        dynamic_cast<ObjectInteger*>(
            term->right->accept(this)
        );
    if (right_int) goto EVALUATE;
    right_float =
        dynamic_cast<ObjectFloat*>(
            term->right->accept(this)
        );
    if (!right_float) {
        std::cerr <<
            "right operand of operator " <<
            term->op.value <<
            " is not numeric \n";
        exit(1);
    }
EVALUATE:
    switch (term->op.ttype) {
        case TOKEN_PLUS: {
            if (left_int && right_int)
                return (*left_int) + right_int;
            else if (left_int && right_float)
                return (*left_int) + right_float;
            else if (left_float && right_int)
                return (*left_float) + right_int;
            return (*left_float) + right_float;
        }
        case TOKEN_MINUS: {
            if (left_int && right_int)
                return (*left_int) - right_int;
            else if (left_int && right_float)
                return (*left_int) - right_float;
            else if (left_float && right_int)
                return (*left_float) - right_int;
            return (*left_float) - right_float;
        }
        default: {
            std::cerr << "Invalid binary operator " << term->op.value << " for numeric operands\n";
            exit(1);
        }
    }
    return nullptr;
}

Object* Interpreter::visit_comparison(Comparison* comparison) {
    ObjectInteger *left_int, *right_int;
    ObjectFloat *left_float, *right_float;
    left_int =
        dynamic_cast<ObjectInteger*>(
            comparison->left->accept(this)
        );
    if (left_int) goto FIND_RIGHT;
    left_float =
        dynamic_cast<ObjectFloat*>(
            comparison->left->accept(this)
        );
    if (!left_float) {
        std::cerr <<
            "Left operand of operator " <<
            comparison->op.value <<
            " is not numeric \n";
        exit(1);
    }
FIND_RIGHT:
    right_int =
        dynamic_cast<ObjectInteger*>(
            comparison->right->accept(this)
        );
    if (right_int) goto EVALUATE;
    right_float =
        dynamic_cast<ObjectFloat*>(
            comparison->right->accept(this)
        );
    if (!right_float) {
        std::cerr <<
            "right operand of operator " <<
            comparison->op.value <<
            " is not numeric \n";
        exit(1);
    }
EVALUATE:
    switch (comparison->op.ttype) {
        case TOKEN_GREATER: {
            if (left_int && right_int)
                return (*left_int) > right_int;
            else if (left_int && right_float)
                return (*left_int) > right_float;
            else if (left_float && right_int)
                return (*left_float) > right_int;
            return (*left_float) > right_float;
        }
        case TOKEN_GREATER_EQUAL: {
            if (left_int && right_int)
                return (*left_int) >= right_int;
            else if (left_int && right_float)
                return (*left_int) >= right_float;
            else if (left_float && right_int)
                return (*left_float) >= right_int;
            return (*left_float) >= right_float;
        }
        case TOKEN_LESS: {
            if (left_int && right_int)
                return (*left_int) < right_int;
            else if (left_int && right_float)
                return (*left_int) < right_float;
            else if (left_float && right_int)
                return (*left_float) < right_int;
            return (*left_float) < right_float;
        }
        case TOKEN_LESS_EQUAL: {
            if (left_int && right_int)
                return (*left_int) <= right_int;
            else if (left_int && right_float)
                return (*left_int) <= right_float;
            else if (left_float && right_int)
                return (*left_float) <= right_int;
            return (*left_float) <= right_float;
        }
        default: {
            std::cerr << "Invalid binary operator " << comparison->op.value << " for numeric operands\n";
            exit(1);
        }
    }
    return nullptr;
}

Object* Interpreter::visit_shift(Shift* shift) {
    ObjectInteger* value =
        dynamic_cast<ObjectInteger*>(shift->left->accept(this));
    if (!value) {
        std::cerr << "Can not shift a non-integer value\n";
        exit(1);
    }
    ObjectInteger* count =
        dynamic_cast<ObjectInteger*>(shift->right->accept(this));
    if (!count || count->value < 0) {
        std::cerr << "Shift count is negative\n";
        exit(1);
    }

    switch (shift->op.ttype) {
        case TOKEN_RIGHT_SHIFT:
            return (*value) >> count;
        case TOKEN_LEFT_SHIFT:
            return (*value) << count;
        default: {
            std::cerr << "Invalid shift operator " << shift->op.value << " for numeric operands\n";
            exit(1);
        }
    }
    return nullptr;
}

Object* Interpreter::visit_equality(Equality* equality) {
    const Object* left =
        equality->left->accept(this);
    const Object* right =
        equality->right->accept(this);
    switch (equality->op.ttype) {
        case TOKEN_LOGICAL_EQUAL:
            return left->equals(right);
        case TOKEN_LOGICAL_NOT_EQUAL:
            return left->equals(right)->negated();
        default: {
            std::cerr << "Invalid equality operator " << equality->op.value << '\n' ;
            exit(1);
        }
    }
    return nullptr;
}

Object* Interpreter::visit_bitwise_and(BitwiseAnd* bitwise_and) {
    const ObjectInteger* left =
        dynamic_cast<const ObjectInteger*>(
            bitwise_and->left->accept(this)
        );
    const ObjectInteger* right =
        dynamic_cast<const ObjectInteger*>(
            bitwise_and->right->accept(this)
        );
    if (!left || !right) {
        std::cerr << "Applying bitwise and `&` to non-integer operands\n";
        exit(1);
    }
    return *left & right;
}

Object* Interpreter::visit_bitwise_xor(BitwiseXor* bitwise_xor) {
    const ObjectInteger* left =
        dynamic_cast<const ObjectInteger*>(
            bitwise_xor->left->accept(this)
        );
    const ObjectInteger* right =
        dynamic_cast<const ObjectInteger*>(
            bitwise_xor->right->accept(this)
        );
    if (!left || !right) {
        std::cerr << "Applying bitwise xor `^` to non-integer operands\n";
        exit(1);
    }
    return *left ^ right;
}

Object* Interpreter::visit_bitwise_or(BitwiseOr* bitwise_or) {
    const ObjectInteger* left =
        dynamic_cast<const ObjectInteger*>(
            bitwise_or->left->accept(this)
        );
    const ObjectInteger* right =
        dynamic_cast<const ObjectInteger*>(
            bitwise_or->right->accept(this)
        );
    if (!left || !right) {
        std::cerr << "Applying bitwise or `|` to non-integer operands\n";
        exit(1);
    }
    return *left | right;
}

Object* Interpreter::visit_logical_and(LogicalAnd* logical_and) {
    const ObjectBoolean* left =
        logical_and->left->accept(this)->to_boolean();
    const ObjectBoolean* right =
        logical_and->right->accept(this)->to_boolean();
    return *left && right;
}

Object* Interpreter::visit_logical_or(LogicalOr* logical_or) {
    const ObjectBoolean* left =
        logical_or->left->accept(this)->to_boolean();
    const ObjectBoolean* right =
        logical_or->right->accept(this)->to_boolean();
    return *left || right;
}
