#include "syntax_tree.hpp"
#include <charconv>
#include <format>
#include <sstream>

std::string Assignment::to_string() const noexcept {
    return std::format(
        "{} = {};", name.value, expr->to_string()
    );
}

InterpreterResult Assignment::accept(Visitor* visitor) {
    return visitor->visit_assignment(this);
}

std::string Return::to_string() const noexcept {
    return std::format("return {} ;", expr->to_string());
}

InterpreterResult Return::accept(Visitor* visitor) {
    return visitor->visit_return(this);
}

std::string Print::to_string() const noexcept {
    return std::format(
        "{} {} ;",
        print_keyword.value,
        expr->to_string()
    );
}

InterpreterResult Print::accept(Visitor* visitor) {
    return visitor->visit_print(this);
}

std::string VariableDeclaration::to_string() const noexcept {
    std::ostringstream fmt;
    fmt << target_type->to_string() ;
    for (auto p : pairs) {
        fmt << p.first ;
        if (p.second)
            fmt << " = " << p.second->to_string() ;
        fmt << ',' ;
    }
    fmt << " ;" ;
    return fmt.str();
}

InterpreterResult VariableDeclaration::accept(Visitor* visitor) {
    return visitor->visit_variable_declaration(this);
}

std::string Cast::to_string() const noexcept {
    return std::format(
        "({}){}",
        this->target_type->to_string(),
        casted_expr->to_string()
    );
}

InterpreterResult Cast::accept(Visitor* visitor) {
    return visitor->visit_cast(this);
}

std::string Block::to_string() const noexcept {
    std::ostringstream fmt;
    fmt << "{" ;
    for (
        auto stmt_ptr = this->statements.begin();
        stmt_ptr != this->statements.end()-1;
        stmt_ptr++
    ) {
        auto tree = *stmt_ptr;
        fmt << tree->to_string() ;
        fmt << ';' ;
    }
    fmt << "}" ;
    return fmt.str();
}

InterpreterResult Block::accept(Visitor* visitor) {
    return visitor->visit_block(this);
}

std::string Program::to_string() const noexcept {
    std::ostringstream fmt;
    for (TreeBase* stmt : statements)
        fmt << stmt->to_string() << '\n' ;
    return fmt.str();
}

InterpreterResult Program::accept(Visitor* visitor) {
    return visitor->visit_program(this);
}

std::string Binary::to_string() const noexcept {
    return std::format(
        "{} {} {}",
        left->to_string(),
        op.value,
        right->to_string()
    );
}

InterpreterResult Logical::accept(Visitor* visitor) {
    return visitor->visit_logical(this);
}

InterpreterResult Bitwise::accept(Visitor* visitor) {
    return visitor->visit_bitwise(this);
}

InterpreterResult Equality::accept(Visitor* visitor) {
    return visitor->visit_equality(this);
}

InterpreterResult Shift::accept(Visitor* visitor) {
    return visitor->visit_shift(this);
}

InterpreterResult Comparison::accept(Visitor* visitor) {
    return visitor->visit_comparison(this);
}

InterpreterResult Term::accept(Visitor* visitor) {
    return visitor->visit_term(this);
}

InterpreterResult Factor::accept(Visitor* visitor) {
    return visitor->visit_factor(this);
}

InterpreterResult Exponential::accept(Visitor* visitor) {
    return visitor->visit_exponential(this);
}

std::string Unary::to_string() const noexcept {
    return std::format(
        "{}{}",
        unary_op.value,
        expr->to_string()
    );
}

InterpreterResult Unary::accept(Visitor* visitor) {
    return visitor->visit_unary(this);
}

std::string Literal::to_string() const noexcept {
    return value_object->to_string();
}

InterpreterResult Literal::accept(Visitor* visitor) {
    return visitor->visit_literal(this);
}

std::string GroupedExpression::to_string() const noexcept {
    return std::format("({})", grouped_expr->to_string());
}

std::string Name::to_string() const noexcept {
    return name_str;
}

InterpreterResult Name::accept(Visitor* visitor) {
    return visitor->visit_name(this);
}

InterpreterResult GroupedExpression::accept(Visitor* visitor) {
    return visitor->visit_grouped_expression(this);
}

