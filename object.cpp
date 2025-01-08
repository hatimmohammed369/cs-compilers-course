#include <cmath>
#include <iomanip>
#include "object.hpp"

// ------------------------- ObjectInteger -------------------------

std::string ObjectInteger::to_string() const noexcept {
    return std::to_string(value);
}

ObjectInteger* ObjectInteger::operator-() const noexcept {
    return new ObjectInteger{-value};
}

ObjectInteger* ObjectInteger::operator*(const ObjectInteger* other) const noexcept {
    return new ObjectInteger{value * other->get()};
}

ObjectFloat* ObjectInteger::operator*(const ObjectFloat* other) const noexcept {
    return new ObjectFloat{
        static_cast<float64>(value) * other->get()
    };
}

ObjectInteger* ObjectInteger::operator%(const ObjectInteger* other) const noexcept {
    if (!other->get()) {
        std::cerr << "Zero modulus\n";
        exit(1);
    }
    return new ObjectInteger {get() % other->get()};
}

ObjectInteger* ObjectInteger::operator+(const ObjectInteger* other) const noexcept {
    return new ObjectInteger {value + other->get()};
}

ObjectFloat* ObjectInteger::operator+(const ObjectFloat* other) const noexcept {
    return new ObjectFloat {static_cast<float64>(value) + other->get()};
}

ObjectInteger* ObjectInteger::operator-(const ObjectInteger* other) const noexcept {
    return new ObjectInteger {value - other->get()};
}

ObjectFloat* ObjectInteger::operator-(const ObjectFloat* other) const noexcept {
    return new ObjectFloat {static_cast<float64>(value) - other->get()};
}

ObjectInteger* ObjectInteger::operator~() const noexcept {
    return new ObjectInteger{~value};
}

ObjectInteger* ObjectInteger::operator>>(const ObjectInteger* count) const noexcept {
    return new ObjectInteger{value >> count->value};
}

ObjectInteger* ObjectInteger::operator<<(const ObjectInteger* count) const noexcept {
    return new ObjectInteger{value << count->value};
}

ObjectInteger* ObjectInteger::operator&(const ObjectInteger* count) const noexcept {
    return new ObjectInteger{value & count->value};
}

ObjectInteger* ObjectInteger::operator^(const ObjectInteger* count) const noexcept {
    return new ObjectInteger{value ^ count->value};
}

ObjectInteger* ObjectInteger::operator|(const ObjectInteger* count) const noexcept {
    return new ObjectInteger{value | count->value};
}

ObjectInteger* ObjectInteger::copy() const noexcept {
    return new ObjectInteger{this->value};
}

// ------------------------- ObjectInteger -------------------------

// ------------------------- ObjectFloat -------------------------

std::string ObjectFloat::to_string() const noexcept {
    fmt << std::setprecision(16) ;
    fmt << value ;
    if (std::floorl(value) == std::ceill(value))
        fmt << ".0" ;
    return read_fmt();
}

ObjectFloat* ObjectFloat::operator-() const noexcept {
    return new ObjectFloat{-value};
}

ObjectFloat* ObjectFloat::operator*(const ObjectInteger* other) const noexcept {
    return new ObjectFloat{
        static_cast<float64>(value * other->get())
    };
}

ObjectFloat* ObjectFloat::operator*(const ObjectFloat* other) const noexcept {
    return new ObjectFloat{
        static_cast<float64>(value * other->get())
    };
}

ObjectFloat* ObjectFloat::operator+(const ObjectInteger* other) const noexcept {
    return new ObjectFloat{value + static_cast<float64>(other->get())};
}

ObjectFloat* ObjectFloat::operator+(const ObjectFloat* other) const noexcept {
    return new ObjectFloat{value + other->get()};
}

ObjectFloat* ObjectFloat::operator-(const ObjectInteger* other) const noexcept {
    return new ObjectFloat{value - static_cast<float64>(other->get())};
}

ObjectFloat* ObjectFloat::operator-(const ObjectFloat* other) const noexcept {
    return new ObjectFloat{value - other->get()};
}

ObjectFloat* ObjectFloat::copy() const noexcept {
    return new ObjectFloat{this->value};
}

// ------------------------- ObjectFloat -------------------------

// ------------------------- ObjectVoid -------------------------

const std::string ObjectVoid::NAME = "void";
ObjectVoid* ObjectVoid::VOID_OBJECT = new ObjectVoid;

std::string ObjectVoid::to_string() const noexcept {
    return ObjectVoid::NAME;
}

ObjectBoolean* ObjectVoid::to_boolean() const noexcept {
    // Void is always false
    return ObjectBoolean::FALSE;
}

ObjectVoid* ObjectVoid::copy() const noexcept {
    return ObjectVoid::VOID_OBJECT;
}

// ------------------------- ObjectVoid -------------------------

// ------------------------- ObjectString -------------------------

std::string ObjectString::to_string() const noexcept {
    return *this;
}

ObjectBoolean* ObjectString::to_boolean() const noexcept {
    // Empty string is false
    // Non-empty string is true
    return ObjectBoolean::as_object(size() != 0);
}

ObjectString* ObjectString::copy() const noexcept {
    return new ObjectString{this->c_str(), this->size()};
}

// ------------------------- ObjectString -------------------------

// ------------------------- ObjectBoolean -------------------------
ObjectBoolean* ObjectBoolean::TRUE = new ObjectBoolean{true};
ObjectBoolean* ObjectBoolean::FALSE = new ObjectBoolean{false};

std::string ObjectBoolean::to_string() const noexcept {
    return str;
}

bool ObjectBoolean::get() const noexcept {
    return value;
}

ObjectBoolean* ObjectBoolean::negated() const noexcept {
    return ObjectBoolean::as_object(!value);
}

ObjectBoolean* ObjectBoolean::operator!() const noexcept {
    return ObjectBoolean::as_object(!value);
}

ObjectBoolean* ObjectBoolean::operator&&(const ObjectBoolean* other) const noexcept {
    return ObjectBoolean::as_object(value && other->value);
}

ObjectBoolean* ObjectBoolean::operator||(const ObjectBoolean* other) const noexcept {
    return ObjectBoolean::as_object(value || other->value);
}

ObjectBoolean* ObjectBoolean::xor_with(const ObjectBoolean* other) const noexcept {
    return ObjectBoolean::as_object(
        (value && !other->value) ||
        (!value && other->value)
    );
}

ObjectBoolean* ObjectBoolean::to_boolean() const noexcept {
    return ObjectBoolean::as_object(this->value);
}

ObjectBoolean* ObjectBoolean::copy() const noexcept {
    return ObjectBoolean::as_object(this->value);
}

// ------------------------- ObjectBoolean -------------------------
