#include <cmath>
#include "interpreter.hpp"
#include "common.hpp"
#include "object.hpp"
#include "token.hpp"
#include "visitor.hpp"

InterpreterResult Interpreter::interpret(TreeBase* tree) {
    return tree->accept(this);
}

InterpreterResult Interpreter::visit_program(Program* tree) {
    if (!tree || tree->statements.empty())
        return InterpreterResult::Ok(nullptr);
    for (
        auto stmt_ptr = tree->statements.begin();
        stmt_ptr != tree->statements.end()-1;
        stmt_ptr++
    ) {
        InterpreterResult r = (*stmt_ptr)->accept(this);
        if (r.is_error()) return r;
    }
    return tree->statements.back()->accept(this);
}

InterpreterResult Interpreter::visit_literal(Literal* tree) {
    return InterpreterResult::Ok(tree->value_object);
}

InterpreterResult Interpreter::visit_grouped_expression(GroupedExpression* tree) {
    return tree->grouped_expr->accept(this);
}

InterpreterResult Interpreter::visit_unary(Unary* tree) {
    InterpreterResult expr_result = tree->expr->accept(this);
    if (expr_result.is_error())
        return expr_result;
    Object* expr = expr_result.unwrap();
    switch (tree->unary_op.ttype) {
        case TokenType::BANG: {
            ObjectBoolean* obj = dynamic_cast<ObjectBoolean*>(expr);
            if (!obj)
                return InterpreterResult::Error("Unary logical operator ! applied to non-boolean") ;
            return InterpreterResult::Ok(obj->negated());
        }
        case TokenType::MINUS: {
            ObjectInteger* int_obj = dynamic_cast<ObjectInteger*>(expr);
            if (int_obj)
                return InterpreterResult::Ok(-(*int_obj));

            ObjectFloat* float_obj = dynamic_cast<ObjectFloat*>(expr);
            if (float_obj)
                return InterpreterResult::Ok(-(*float_obj));

            return InterpreterResult::Error("Unary arithmetic operator - applied to non-numeric") ;
        }
        case TokenType::PLUS: {
            ObjectInteger* int_obj = dynamic_cast<ObjectInteger*>(expr);
            if (int_obj)
                return InterpreterResult::Ok(int_obj);

            ObjectFloat* float_obj = dynamic_cast<ObjectFloat*>(expr);
            if (float_obj)
                return InterpreterResult::Ok(float_obj);

            return InterpreterResult::Error("Unary arithmetic operator + applied to non-numeric") ;
        }
        case TokenType::TILDE: {
            ObjectInteger* int_obj = dynamic_cast<ObjectInteger*>(expr);
            if (!int_obj)
                return InterpreterResult::Error("Unary bitwise operator ~ applied to non-integer") ;
            return InterpreterResult::Ok(~(*int_obj));
        }
        default: {}
    }
    return InterpreterResult::Error(
        "Invalid unary operator " + tree->unary_op.value
    );
}

InterpreterResult Interpreter::visit_exponential(Exponential* tree) {
    InterpreterResult base_result = tree->left->accept(this);
    if (base_result.is_error())
        return base_result;
    Object* base = base_result.unwrap();

    InterpreterResult exponent_result = tree->right->accept(this);
    if (exponent_result.is_error())
        return exponent_result;
    Object* exponent = exponent_result.unwrap();

    ObjectInteger *int_base, *int_exponent;
    ObjectFloat *float_base, *float_exponent;

    int_base = dynamic_cast<ObjectInteger*>(base);
    if (int_base) goto FIND_EXPONENT;
    float_base = dynamic_cast<ObjectFloat*>(base);
    if (!float_base)
        return InterpreterResult::Error("Numeric operator ** used with non-numeric base\n");
FIND_EXPONENT:
    int_exponent = dynamic_cast<ObjectInteger*>(exponent);
    if (int_exponent) goto EVALUATE;
    float_exponent = dynamic_cast<ObjectFloat*>(exponent);
    if (!float_exponent)
        return InterpreterResult::Error("Numeric operator ** used with non-numeric exponent\n");
EVALUATE:
    Object* value;
    if (int_base && int_exponent) {
        value = new ObjectInteger{
            static_cast<i64>(std::powl(int_base->value, int_exponent->value))
        };
    } else if (int_base && float_exponent) {
        value = new ObjectFloat{
            static_cast<float64>(std::pow(int_base->value, float_exponent->value))
        };
    } else if (float_base && int_exponent) {
        value = new ObjectFloat{
            static_cast<float64>(std::pow(float_base->value, int_exponent->value))
        };
    } else {
        // float base && float_exponent
        value = new ObjectFloat{
            static_cast<float64>(std::pow(float_base->value, float_exponent->value))
        };
    }
    return InterpreterResult::Ok(value);
}

InterpreterResult Interpreter::visit_factor(Factor* tree) {
    InterpreterResult left_result = tree->left->accept(this);
    if (left_result.is_error())
        return left_result;
    Object* left = left_result.unwrap();

    InterpreterResult right_result = tree->right->accept(this);
    if (right_result.is_error())
        return right_result;
    Object* right = right_result.unwrap();

    ObjectInteger *left_int, *right_int;
    ObjectFloat *left_float, *right_float;

    left_int = dynamic_cast<ObjectInteger*>(left);
    if (left_int) goto FIND_RIGHT;
    left_float = dynamic_cast<ObjectFloat*>(left);
    if (!left_float) {
        return InterpreterResult::Error(
            "Left operand of operator " +
            tree->op.value +
            " is not numeric \n"
        );
    }
FIND_RIGHT:
    right_int = dynamic_cast<ObjectInteger*>(right);
    if (right_int) goto EVALUATE;
    right_float = dynamic_cast<ObjectFloat*>(right);
    if (!right_float) {
        return InterpreterResult::Error(
            "right operand of operator " +
            tree->op.value +
            " is not numeric \n"
        );
    }
EVALUATE:
    Object* value = nullptr;
    switch (tree->op.ttype) {
        case TokenType::STAR: {
            if (left_int && right_int)
                value = (*left_int) * right_int;
            else if (left_int && right_float)
                value = (*left_int) * right_float;
            else if (left_float && right_int)
                value = (*left_float) * right_int;
            else
                value = (*left_float) * right_float;
            break;
        }
        case TokenType::SLASH: {
            if (left_int && right_int)
                value = (*left_int) / right_int;
            else if (left_int && right_float)
                value = (*left_int) / right_float;
            else if (left_float && right_int)
                value = (*left_float) / right_int;
            else
                value = (*left_float) / right_float;
            if (!value)
                return InterpreterResult::Error("Division by zero\n");
            break;
        }
        case TokenType::DOUBLE_SLASH: {
            if (left_int && right_int)
                value = left_int->integer_div(right_int);
            else if (left_int && right_float)
                value = left_int->integer_div(right_float);
            else if (left_float && right_int)
                value = left_float->integer_div(right_int);
            else
                value = left_float->integer_div(right_float);
            break;
        }
        case TokenType::PERCENT: {
            if (!(left_int && right_int)) {
                return InterpreterResult::Error(
                    "Applying mod operator % with a non-integer operand\n"
                );
            }
            value = (*left_int) % right_int;
            if (!value)
                return InterpreterResult::Error("Zero modulus");
            break;
        }
        default: {}
    }
    if (value) return InterpreterResult::Ok(value);
    return InterpreterResult::Error(
        "Invalid binary operator " + tree->op.value + " for numeric operands\n"
    );
}

InterpreterResult Interpreter::visit_term(Term* tree) {
    InterpreterResult left_result = tree->left->accept(this);
    if (left_result.is_error())
        return left_result;
    Object* left = left_result.unwrap();

    InterpreterResult right_result = tree->right->accept(this);
    if (right_result.is_error())
        return right_result;
    Object* right = right_result.unwrap();

    ObjectString* left_str = dynamic_cast<ObjectString*>(left);
    if (left_str) {
        return InterpreterResult::Ok(
            new ObjectString{*left_str + right->to_string()}
        );
    }
    ObjectString* right_str = dynamic_cast<ObjectString*>(right);
    if (right_str) {
        return InterpreterResult::Ok(
            new ObjectString{left->to_string() + *right_str}
        );
    }

    ObjectInteger *left_int, *right_int;
    ObjectFloat *left_float, *right_float;

    left_int = dynamic_cast<ObjectInteger*>(left);
    if (left_int) goto FIND_RIGHT;
    left_float = dynamic_cast<ObjectFloat*>(left);
    if (!left_float) {
        return InterpreterResult::Error(
            "Left operand of operator " +
            tree->op.value +
            " is not numeric\n"
        );
    }
FIND_RIGHT:
    right_int = dynamic_cast<ObjectInteger*>(right);
    if (right_int) goto EVALUATE;
    right_float = dynamic_cast<ObjectFloat*>(right);
    if (!right_float) {
        return InterpreterResult::Error(
            "right operand of operator " +
            tree->op.value +
            " is not numeric\n"
        );
    }
EVALUATE:
    Object* value = nullptr;
    switch (tree->op.ttype) {
        case TokenType::PLUS: {
            if (left_int && right_int)
                value = (*left_int) + right_int;
            else if (left_int && right_float)
                value = (*left_int) + right_float;
            else if (left_float && right_int)
                value = (*left_float) + right_int;
            else
                value = (*left_float) + right_float;
            break;
        }
        case TokenType::MINUS: {
            if (left_int && right_int)
                value = (*left_int) - right_int;
            else if (left_int && right_float)
                value = (*left_int) - right_float;
            else if (left_float && right_int)
                value = (*left_float) - right_int;
            else
                value = (*left_float) - right_float;
            break;
        }
        default: {}
    }
    if (value) return InterpreterResult::Ok(value);
    return InterpreterResult::Error(
        "Invalid binary operator " + tree->op.value + " for numeric operands\n"
    );
}

InterpreterResult Interpreter::visit_comparison(Comparison* tree) {
    InterpreterResult left_result = tree->left->accept(this);
    if (left_result.is_error())
        return left_result;
    Object* left = left_result.unwrap();

    InterpreterResult right_result = tree->right->accept(this);
    if (right_result.is_error())
        return right_result;
    Object* right = right_result.unwrap();

    ObjectInteger *left_int, *right_int;
    ObjectFloat *left_float, *right_float;

    left_int = dynamic_cast<ObjectInteger*>(left);
    if (left_int) goto FIND_RIGHT;
    left_float = dynamic_cast<ObjectFloat*>(left);
    if (!left_float) {
        return InterpreterResult::Error(
            "Left operand of operator " +
            tree->op.value +
            " is not numeric\n"
        );
    }
FIND_RIGHT:
    right_int = dynamic_cast<ObjectInteger*>(right);
    if (right_int) goto EVALUATE;
    right_float = dynamic_cast<ObjectFloat*>(right);
    if (!right_float) {
        return InterpreterResult::Error(
            "right operand of operator " +
            tree->op.value +
            " is not numeric \n"
        );
    }
EVALUATE:
    Object* value = nullptr;
    switch (tree->op.ttype) {
        case TokenType::GREATER: {
            if (left_int && right_int)
                value = (*left_int) > right_int;
            else if (left_int && right_float)
                value = (*left_int) > right_float;
            else if (left_float && right_int)
                value = (*left_float) > right_int;
            else
                value = (*left_float) > right_float;
            break;
        }
        case TokenType::GREATER_EQUAL: {
            if (left_int && right_int)
                value = (*left_int) >= right_int;
            else if (left_int && right_float)
                value = (*left_int) >= right_float;
            else if (left_float && right_int)
                value = (*left_float) >= right_int;
            else
                value = (*left_float) >= right_float;
            break;
        }
        case TokenType::LESS: {
            if (left_int && right_int)
                value = (*left_int) < right_int;
            else if (left_int && right_float)
                value = (*left_int) < right_float;
            else if (left_float && right_int)
                value = (*left_float) < right_int;
            else
                value = (*left_float) < right_float;
            break;
        }
        case TokenType::LESS_EQUAL: {
            if (left_int && right_int)
                value = (*left_int) <= right_int;
            else if (left_int && right_float)
                value = (*left_int) <= right_float;
            else if (left_float && right_int)
                value = (*left_float) <= right_int;
            else
                value = (*left_float) <= right_float;
            break;
        }
        default: {}
    }
    if (value) return InterpreterResult::Ok(value);
    return InterpreterResult::Error(
        "Invalid binary operator " + tree->op.value + " for numeric operands\n"
    );
}

InterpreterResult Interpreter::visit_shift(Shift* tree) {
    InterpreterResult left_result = tree->left->accept(this);
    if (left_result.is_error())
        return left_result;
    Object* left = left_result.unwrap();

    InterpreterResult right_result = tree->right->accept(this);
    if (right_result.is_error())
        return right_result;
    Object* right = right_result.unwrap();

    ObjectInteger* value = dynamic_cast<ObjectInteger*>(left);
    if (!value) {
        return InterpreterResult::Error(
            "Can not shift a non-integer value\n"
        );
    }
    ObjectInteger* count = dynamic_cast<ObjectInteger*>(right);
    if (!count || count->value < 0) {
        return InterpreterResult::Error(
            "Shift count is negative\n"
        );
    }
    switch (tree->op.ttype) {
        case TokenType::RIGHT_SHIFT:
            return InterpreterResult::Ok((*value) >> count);
        case TokenType::LEFT_SHIFT:
            return InterpreterResult::Ok((*value) << count);
        default: {}
    }
    return InterpreterResult::Error(
        "Invalid shift operator " + tree->op.value + " for numeric operands\n"
    );
}

InterpreterResult Interpreter::visit_equality(Equality* tree) {
    InterpreterResult left_result = tree->left->accept(this);
    if (left_result.is_error())
        return left_result;
    const Object* left = left_result.unwrap();

    InterpreterResult right_result = tree->right->accept(this);
    if (right_result.is_error())
        return right_result;
    const Object* right = right_result.unwrap();

    switch (tree->op.ttype) {
        case TokenType::LOGICAL_EQUAL: {
            return InterpreterResult::Ok(
                left->equals(right)
            );
        }
        case TokenType::LOGICAL_NOT_EQUAL: {
            return InterpreterResult::Ok(
                left->equals(right)->negated()
            );
        }
        default: {}
    }
    return InterpreterResult::Error(
        "Invalid equality operator " + tree->op.value + '\n'
    );
}

InterpreterResult Interpreter::visit_bitwise(Bitwise* tree) {
    InterpreterResult left_result = tree->left->accept(this);
    if (left_result.is_error())
        return left_result;
    const ObjectInteger* left =
        dynamic_cast<const ObjectInteger*>(left_result.unwrap());

    InterpreterResult right_result = tree->right->accept(this);
    if (right_result.is_error())
        return right_result;
    const ObjectInteger* right =
        dynamic_cast<const ObjectInteger*>(right_result.unwrap());

    if (!left || !right) {
        return InterpreterResult::Error(
            "Applying bitwise `"
            + tree->op.value
            + "` to non-integer operands\n"
        );
    }
    Object* value = nullptr;
    switch (tree->op.ttype) {
        case TokenType::BITWISE_XOR: {
            value = *left ^ right;
            break;
        }
        case TokenType::BITWISE_OR: {
            value = *left | right;
            break;
        }
        case TokenType::BITWISE_AND: {
            value = *left & right;
            break;
        }
        default: {}
    }
    if (value) return InterpreterResult::Ok(value);
    return InterpreterResult::Error(
        "Invalid bitwise operator `" + tree->op.value + "`\n"
    );
}

InterpreterResult Interpreter::visit_logical(Logical* tree) {
    InterpreterResult left_result = tree->left->accept(this);
    if (left_result.is_error())
        return left_result;
    const ObjectBoolean* left =
        left_result.unwrap()->to_boolean();

    InterpreterResult right_result = tree->right->accept(this);
    if (right_result.is_error())
        return right_result;
    const ObjectBoolean* right =
        right_result.unwrap()->to_boolean();

    Object* value = nullptr;
    switch (tree->op.ttype) {
        case TokenType::KEYWORD_XOR: {
            value = left->xor_with(right);
            break;
        }
        case TokenType::KEYWORD_OR: {
            value = *left || right;
            break;
        }
        case TokenType::KEYWORD_AND: {
            value = *left && right;
            break;
        }
        default: {}
    }
    if (value) return InterpreterResult::Ok(value);
    return InterpreterResult::Error(
        "Invalid logical operator `" + tree->op.value + "`\n"
    );
}

InterpreterResult Interpreter::visit_block(Block* tree) {
    if (!tree || tree->statements.empty())
        return InterpreterResult::Ok(nullptr);
    env.begin_scope();
    Object* return_value = ObjectVoid::VOID_OBJECT;
    for (Statement* stmt : tree->statements) {
        InterpreterResult stmt_result = stmt->accept(this);
        if (stmt_result.is_error())
            return stmt_result;
        if (dynamic_cast<Return*>(stmt)) {
            return_value = stmt_result.unwrap();
            break;
        }
    }
    env.end_scope();
    return InterpreterResult::Ok(return_value);
}

InterpreterResult Interpreter::visit_cast(Cast* tree) {
    InterpreterResult expr_result =
        tree->casted_expr->accept(this);
    if (expr_result.is_error())
        return expr_result;
    Object* cast_return =
        tree->target_type->cast(
            expr_result.unwrap()
        );
    if (!cast_return) {
        return InterpreterResult::Error(
            std::format(
                "Object of type `{}` can not be casted to object of type `{}`",
                expr_result.unwrap()->type_info->to_string(),
                tree->target_type->to_string()
            )
        );
    }
    return InterpreterResult::Ok(cast_return);
}

InterpreterResult Interpreter::visit_variable_declaration(VariableDeclaration* tree) {
    for (
        auto stmt_ptr = tree->pairs.begin();
        stmt_ptr != tree->pairs.end();
        stmt_ptr++
    ) {
        std::pair<std::string, TreeBase*> p = *stmt_ptr;
        std::string name = p.first;
        env.define(name);
        Object* value = ObjectVoid::VOID_OBJECT;
        if (p.second) {
            InterpreterResult initializer_result =
                p.second->accept(this);
            if (initializer_result.is_error())
                return initializer_result;
            else
                value = initializer_result.unwrap();
        }
        env.set(name, value);
    }
    return InterpreterResult::Ok(nullptr);
}

InterpreterResult Interpreter::visit_print(Print* tree) {
    InterpreterResult expr_result =
        tree->expr->accept(this);
    if (expr_result.is_error())
        return expr_result;
    std::string expr_str =
        expr_result.unwrap()->to_string();
    std::cout << expr_str ;
    if (tree->print_keyword.ttype == TokenType::KEYWORD_PRINTLN)
        std::cout << '\n' ;
    if (is_mode_interactive())
        std::cout << '\n';
    return InterpreterResult::Ok(nullptr);
}

InterpreterResult Interpreter::visit_return(Return* tree) {
    return tree->expr->accept(this);
}

InterpreterResult Interpreter::visit_name(Name* tree) {
    EnvironmentResult get_result = env.get(tree->name_str);
    if (get_result.is_error())
        return InterpreterResult::Error(get_result.unwrap_error());
    return InterpreterResult::Ok(get_result.unwrap());
}
