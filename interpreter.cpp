#include <cmath>
#include "interpreter.hpp"
#include "common.hpp"
#include "object.hpp"
#include "token.hpp"

Object* Interpreter::interpret(TreeBase* tree) {
    return tree->accept(this);
}

Object* Interpreter::visit_program(Program* tree) {
    if (!tree || tree->statements.empty())
        return nullptr;
    for (
        auto stmt_ptr = tree->statements.begin();
        stmt_ptr != tree->statements.end()-1;
        stmt_ptr++
    ) {
        (void)(*stmt_ptr)->accept(this);
    }
    return tree->statements.back()->accept(this);
}

Object* Interpreter::visit_literal(Literal* tree) {
    return tree->value_object;
}

Object* Interpreter::visit_grouped_expression(GroupedExpression* tree) {
    return tree->grouped_expr->accept(this);
}

Object* Interpreter::visit_unary(Unary* tree) {
    switch (tree->unary_op.ttype) {
        case TokenType::BANG: {
            ObjectBoolean* obj =
                dynamic_cast<ObjectBoolean*>(tree->expr->accept(this));
            if (!obj) {
                std::cerr << "Unary logical operator ! applied to non-boolean" ;
                exit(1);
            }
            return obj->negated();
        }
        case TokenType::MINUS: {
            Object* expr_value = tree->expr->accept(this);
            ObjectInteger* int_obj =
                dynamic_cast<ObjectInteger*>(expr_value);
            if (int_obj)
                return -(*int_obj);

            ObjectFloat* float_obj =
                dynamic_cast<ObjectFloat*>(expr_value);
            if (float_obj)
                return -(*float_obj);

            std::cerr << "Unary arithmetic operator - applied to non-numeric" ;
            exit(1);
        }
        case TokenType::PLUS: {
            Object* expr_value = tree->expr->accept(this);
            ObjectInteger* int_obj =
                dynamic_cast<ObjectInteger*>(expr_value);
            if (int_obj)
                return int_obj;

            ObjectFloat* float_obj =
                dynamic_cast<ObjectFloat*>(expr_value);
            if (float_obj)
                return float_obj;

            std::cerr << "Unary arithmetic operator + applied to non-numeric" ;
            exit(1);
        }
        case TokenType::TILDE: {
            ObjectInteger* int_obj =
                dynamic_cast<ObjectInteger*>(tree->expr->accept(this));
            if (!int_obj) {
                std::cerr << "Unary bitwise operator ~ applied to non-integer" ;
                exit(1);
            }
            return ~(*int_obj);
        }
        default: {
            std::cerr << "Invalid unary operator " << tree->unary_op.value << '\n';
            exit(1);
        }
    }
    return nullptr;
}

Object* Interpreter::visit_exponential(Exponential* tree) {
    ObjectInteger *int_base, *int_exponent;
    ObjectFloat *float_base, *float_exponent;
    int_base =
        dynamic_cast<ObjectInteger*>(tree->left->accept(this));
    if (int_base) goto FIND_EXPONENT;
    float_base =
        dynamic_cast<ObjectFloat*>(tree->left->accept(this));
    if (!float_base) {
        std::cerr << "Numeric operator ** used with non-numeric base\n" ;
        exit(1);
    }
FIND_EXPONENT:
    int_exponent =
        dynamic_cast<ObjectInteger*>(tree->right->accept(this));
    if (int_exponent) goto EVALUATE;
    float_exponent =
        dynamic_cast<ObjectFloat*>(tree->right->accept(this));
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

Object* Interpreter::visit_factor(Factor* tree) {
    ObjectInteger *left_int, *right_int;
    ObjectFloat *left_float, *right_float;
    left_int =
        dynamic_cast<ObjectInteger*>(
            tree->left->accept(this)
        );
    if (left_int) goto FIND_RIGHT;
    left_float =
        dynamic_cast<ObjectFloat*>(
            tree->left->accept(this)
        );
    if (!left_float) {
        std::cerr <<
            "Left operand of operator " <<
            tree->op.value <<
            " is not numeric \n";
        exit(1);
    }
FIND_RIGHT:
    right_int =
        dynamic_cast<ObjectInteger*>(
            tree->right->accept(this)
        );
    if (right_int) goto EVALUATE;
    right_float =
        dynamic_cast<ObjectFloat*>(
            tree->right->accept(this)
        );
    if (!right_float) {
        std::cerr <<
            "right operand of operator " <<
            tree->op.value <<
            " is not numeric \n";
        exit(1);
    }
EVALUATE:
    switch (tree->op.ttype) {
        case TokenType::STAR: {
            if (left_int && right_int)
                return (*left_int) * right_int;
            else if (left_int && right_float)
                return (*left_int) * right_float;
            else if (left_float && right_int)
                return (*left_float) * right_int;
            return (*left_float) * right_float;
        }
        case TokenType::SLASH: {
            if (left_int && right_int)
                return (*left_int) / right_int;
            else if (left_int && right_float)
                return (*left_int) / right_float;
            else if (left_float && right_int)
                return (*left_float) / right_int;
            return (*left_float) / right_float;
        }
        case TokenType::DOUBLE_SLASH: {
            if (left_int && right_int)
                return left_int->integer_div(right_int);
            else if (left_int && right_float)
                return left_int->integer_div(right_float);
            else if (left_float && right_int)
                return left_float->integer_div(right_int);
            return left_float->integer_div(right_float);
        }
        case TokenType::PERCENT: {
            if (!(left_int && right_int)) {
                std::cerr << "Applying mod operator % with a non-integer operand\n";
                exit(1);
            }
            return (*left_int) % right_int;
        }
        default: {
            std::cerr << "Invalid binary operator " << tree->op.value << " for numeric operands\n";
            exit(1);
        }
    }
    return nullptr;
}

Object* Interpreter::visit_term(Term* tree) {
    ObjectInteger *left_int, *right_int;
    ObjectFloat *left_float, *right_float;
    left_int =
        dynamic_cast<ObjectInteger*>(
            tree->left->accept(this)
        );
    if (left_int) goto FIND_RIGHT;
    left_float =
        dynamic_cast<ObjectFloat*>(
            tree->left->accept(this)
        );
    if (!left_float) {
        std::cerr <<
            "Left operand of operator " <<
            tree->op.value <<
            " is not numeric \n";
        exit(1);
    }
FIND_RIGHT:
    right_int =
        dynamic_cast<ObjectInteger*>(
            tree->right->accept(this)
        );
    if (right_int) goto EVALUATE;
    right_float =
        dynamic_cast<ObjectFloat*>(
            tree->right->accept(this)
        );
    if (!right_float) {
        std::cerr <<
            "right operand of operator " <<
            tree->op.value <<
            " is not numeric \n";
        exit(1);
    }
EVALUATE:
    switch (tree->op.ttype) {
        case TokenType::PLUS: {
            if (left_int && right_int)
                return (*left_int) + right_int;
            else if (left_int && right_float)
                return (*left_int) + right_float;
            else if (left_float && right_int)
                return (*left_float) + right_int;
            return (*left_float) + right_float;
        }
        case TokenType::MINUS: {
            if (left_int && right_int)
                return (*left_int) - right_int;
            else if (left_int && right_float)
                return (*left_int) - right_float;
            else if (left_float && right_int)
                return (*left_float) - right_int;
            return (*left_float) - right_float;
        }
        default: {
            std::cerr << "Invalid binary operator " << tree->op.value << " for numeric operands\n";
            exit(1);
        }
    }
    return nullptr;
}

Object* Interpreter::visit_comparison(Comparison* tree) {
    ObjectInteger *left_int, *right_int;
    ObjectFloat *left_float, *right_float;
    left_int =
        dynamic_cast<ObjectInteger*>(
            tree->left->accept(this)
        );
    if (left_int) goto FIND_RIGHT;
    left_float =
        dynamic_cast<ObjectFloat*>(
            tree->left->accept(this)
        );
    if (!left_float) {
        std::cerr <<
            "Left operand of operator " <<
            tree->op.value <<
            " is not numeric \n";
        exit(1);
    }
FIND_RIGHT:
    right_int =
        dynamic_cast<ObjectInteger*>(
            tree->right->accept(this)
        );
    if (right_int) goto EVALUATE;
    right_float =
        dynamic_cast<ObjectFloat*>(
            tree->right->accept(this)
        );
    if (!right_float) {
        std::cerr <<
            "right operand of operator " <<
            tree->op.value <<
            " is not numeric \n";
        exit(1);
    }
EVALUATE:
    switch (tree->op.ttype) {
        case TokenType::GREATER: {
            if (left_int && right_int)
                return (*left_int) > right_int;
            else if (left_int && right_float)
                return (*left_int) > right_float;
            else if (left_float && right_int)
                return (*left_float) > right_int;
            return (*left_float) > right_float;
        }
        case TokenType::GREATER_EQUAL: {
            if (left_int && right_int)
                return (*left_int) >= right_int;
            else if (left_int && right_float)
                return (*left_int) >= right_float;
            else if (left_float && right_int)
                return (*left_float) >= right_int;
            return (*left_float) >= right_float;
        }
        case TokenType::LESS: {
            if (left_int && right_int)
                return (*left_int) < right_int;
            else if (left_int && right_float)
                return (*left_int) < right_float;
            else if (left_float && right_int)
                return (*left_float) < right_int;
            return (*left_float) < right_float;
        }
        case TokenType::LESS_EQUAL: {
            if (left_int && right_int)
                return (*left_int) <= right_int;
            else if (left_int && right_float)
                return (*left_int) <= right_float;
            else if (left_float && right_int)
                return (*left_float) <= right_int;
            return (*left_float) <= right_float;
        }
        default: {
            std::cerr << "Invalid binary operator " << tree->op.value << " for numeric operands\n";
            exit(1);
        }
    }
    return nullptr;
}

Object* Interpreter::visit_shift(Shift* tree) {
    ObjectInteger* value =
        dynamic_cast<ObjectInteger*>(tree->left->accept(this));
    if (!value) {
        std::cerr << "Can not shift a non-integer value\n";
        exit(1);
    }
    ObjectInteger* count =
        dynamic_cast<ObjectInteger*>(tree->right->accept(this));
    if (!count || count->value < 0) {
        std::cerr << "Shift count is negative\n";
        exit(1);
    }

    switch (tree->op.ttype) {
        case TokenType::RIGHT_SHIFT:
            return (*value) >> count;
        case TokenType::LEFT_SHIFT:
            return (*value) << count;
        default: {
            std::cerr << "Invalid shift operator " << tree->op.value << " for numeric operands\n";
            exit(1);
        }
    }
    return nullptr;
}

Object* Interpreter::visit_equality(Equality* tree) {
    const Object* left =
        tree->left->accept(this);
    const Object* right =
        tree->right->accept(this);
    switch (tree->op.ttype) {
        case TokenType::LOGICAL_EQUAL:
            return left->equals(right);
        case TokenType::LOGICAL_NOT_EQUAL:
            return left->equals(right)->negated();
        default: {
            std::cerr << "Invalid equality operator " << tree->op.value << '\n' ;
            exit(1);
        }
    }
    return nullptr;
}

Object* Interpreter::visit_bitwise(Bitwise* tree) {
    const ObjectInteger* left =
        dynamic_cast<const ObjectInteger*>(
            tree->left->accept(this)
        );
    const ObjectInteger* right =
        dynamic_cast<const ObjectInteger*>(
            tree->right->accept(this)
        );
    if (!left || !right) {
        std::cerr << "Applying bitwise `"
            << tree->op.value
            << "` to non-integer operands\n";
        exit(1);
    }
    switch (tree->op.ttype) {
        case TokenType::BITWISE_XOR:
            return *left ^ right;
        case TokenType::BITWISE_OR:
            return *left | right;
        case TokenType::BITWISE_AND:
            return *left & right;
        default: {
            std::cerr << "Invalid bitwise operator `" << tree->op.value << "`\n";
            exit(1);
        }
    }
    return nullptr;
}

Object* Interpreter::visit_logical(Logical* tree) {
    const ObjectBoolean* left =
        tree->left->accept(this)->to_boolean();
    const ObjectBoolean* right =
        tree->right->accept(this)->to_boolean();
    switch (tree->op.ttype) {
        case TokenType::KEYWORD_XOR:
            return left->xor_with(right);
        case TokenType::KEYWORD_OR:
            return *left || right;
        case TokenType::KEYWORD_AND:
            return *left && right;
        default: {
            std::cerr << "Invalid logical operator `" << tree->op.value << "`\n";
            exit(1);
        }
    }
    return nullptr;
}

Object* Interpreter::visit_block(Block* tree) {
    if (!tree || tree->statements.empty())
        return nullptr;
    for (
        auto stmt_ptr = tree->statements.begin();
        stmt_ptr != tree->statements.end()-1;
        stmt_ptr++
    ) {
        (void)(*stmt_ptr)->accept(this);
    }
    return tree->statements.back()->accept(this);
}

Object* Interpreter::visit_cast(Cast* tree) {
    return tree->target_type->cast(
        tree->casted_expr->accept(this)
    );
}

Object* Interpreter::visit_variable_declaration(VariableDeclaration* tree) {
    for (
        auto stmt_ptr = tree->pairs.begin();
        stmt_ptr != tree->pairs.end();
        stmt_ptr++
    ) {
        std::pair<std::string, TreeBase*> p = *stmt_ptr;
        std::string name = p.first;
        Object* value = (
            p.second ? p.second->accept(this) : ObjectVoid::VOID_OBJECT
        );
        defined_names[name] = value;
    }
    return nullptr;
}

Object* Interpreter::visit_print(Print* tree) {
    std::string expr_str =
        tree->expr->accept(this)->to_string();
    std::cout << expr_str ;
    if (tree->print_keyword.ttype == TokenType::KEYWORD_PRINTLN)
        std::cout << '\n' ;
    return nullptr;
}

Object* Interpreter::visit_return(Return* tree) {
    return tree->accept(this);
}

Object* Interpreter::visit_name(Name* tree) {
    Object* name_val = defined_names[tree->name_str];
    if (!name_val) {
        std::cerr << "Undefined name '" << tree->name_str << "'\n";
        exit(1);
    }
    return name_val;
}
