#include <cmath>
#include <iomanip>
#include <sstream>
#include "object.hpp"

// ------------------------- Number -------------------------

template <>
ObjectBoolean* Number<i64>::equals(const Object* other) const noexcept {
    if (
        other->get_tag() != OBJECT_INTEGER &&
        other->get_tag() != OBJECT_FLOAT
    )
        return ObjectBoolean::get_false_object();

    const ObjectInteger* other_int =
        dynamic_cast<const ObjectInteger*>(other);
    if (other_int)
        return ObjectBoolean::as_object(
            this->value == other_int->get()
        );

    const ObjectFloat* other_float =
        dynamic_cast<const ObjectFloat*>(other);
    return ObjectBoolean::as_object(
        this->value == other_float->get()
    );
}

template <>
ObjectBoolean* Number<float64>::equals(const Object* other) const noexcept {
    if (
        other->get_tag() != OBJECT_INTEGER &&
        other->get_tag() != OBJECT_FLOAT
    )
        return ObjectBoolean::get_false_object();

    const ObjectInteger* other_int =
        dynamic_cast<const ObjectInteger*>(other);
    if (other_int)
        return ObjectBoolean::as_object(
            this->value == other_int->get()
        );

    const ObjectFloat* other_float =
        dynamic_cast<const ObjectFloat*>(other);
    return ObjectBoolean::as_object(
        this->value == other_float->get()
    );
}

template <typename T>
Number<i64>* Number<T>::integer_div(const Number<i64>* other) const noexcept {
    if (!other->get()) {
        std::cerr << "Division by zero\n";
        exit(1);
    }
    return new ObjectInteger {
        static_cast<i64>(get()) / other->get()
    };
}

template <typename T>
Number<i64>* Number<T>::integer_div(const Number<float64>* other) const noexcept {
    if (!other->get()) {
        std::cerr << "Division by zero\n";
        exit(1);
    }
    return new ObjectInteger {
        static_cast<i64>(get()) / static_cast<i64>(other->get())
    };
}

template <typename T>
Number<float64>* Number<T>::operator/(const Number<i64>* other) const noexcept {
    if (!other->get()) {
        std::cerr << "Division by zero\n";
        exit(1);
    }
    return new ObjectFloat {
        static_cast<float64>(get()) / static_cast<float64>(other->get())
    };
}

template <typename T>
Number<float64>* Number<T>::operator/(const Number<float64>* other) const noexcept {
    if (!other->get()) {
        std::cerr << "Division by zero\n";
        exit(1);
    }
    return new ObjectFloat {
        static_cast<float64>(get()) / static_cast<float64>(other->get())
    };
}

template <typename T>
ObjectBoolean* Number<T>::to_boolean() const noexcept {
    // Zero is false
    // Non-zero is true
    return ObjectBoolean::as_object(this->value != 0);
}

// ------------------------- Number -------------------------

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

// ------------------------- ObjectInteger -------------------------

// ------------------------- ObjectFloat -------------------------

std::string ObjectFloat::to_string() const noexcept {
    std::ostringstream oss;
    oss << std::setprecision(16) ;
    oss << value ;
    if (std::floorl(value) == std::ceill(value))
        oss << ".0" ;
    return oss.str();
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

// ------------------------- ObjectFloat -------------------------

// ------------------------- ObjectVoid -------------------------

std::string ObjectVoid::to_string() const noexcept {
    return ObjectVoid::get_string_value();
}

ObjectBoolean* ObjectVoid::equals(const Object* other) const noexcept {
    return ObjectBoolean::as_object(
        other->get_tag() == OBJECT_VOID
    );
} 

ObjectBoolean* ObjectVoid::to_boolean() const noexcept {
    // Void is always false
    return ObjectBoolean::get_false_object();
}

// ------------------------- ObjectVoid -------------------------

// ------------------------- ObjectString -------------------------

ObjectString::ObjectString() {
    tag = OBJECT_STRING;
    chars = nullptr;
    _length = 0;
}

ObjectString::ObjectString(const char* s, size_t len) {
    tag = OBJECT_STRING;
    chars = new char[len + 1];
    chars[len] = '\0';
    std::strncpy(chars, s, len);
    _length = len;
}

ObjectString::ObjectString(const char* s): ObjectString(s, strlen(s)) {}

ObjectString::ObjectString(const std::string& s):
    ObjectString(s.c_str(), s.length()) {}

ObjectString::~ObjectString() {delete chars;}

ObjectBoolean* ObjectString::equals(const Object* other) const noexcept {
    const ObjectString* str =
        dynamic_cast<const ObjectString*>(other);
    return ObjectBoolean::as_object(
        other->get_tag() == OBJECT_STRING &&
        this->_length == str->_length &&
        std::strncmp(this->chars, str->chars, this->_length) == 0
    );
} 

std::string ObjectString::to_string() const noexcept {
    std::ostringstream oss;
    oss << '"' ;
    oss << std::string(chars, _length) ;
    oss << '"' ;
    return oss.str();
}

const char* ObjectString::get() const noexcept {
    return chars;
}

size_t ObjectString::length() const noexcept {
    return _length;
}

ObjectBoolean* ObjectString::to_boolean() const noexcept {
    // Empty string is false
    // Non-empty string is true
    return ObjectBoolean::as_object(this->_length != 0);
}

// ------------------------- ObjectString -------------------------

// ------------------------- ObjectBoolean -------------------------

ObjectBoolean* ObjectBoolean::equals(const Object* other) const noexcept {
    const ObjectBoolean* boolean =
        dynamic_cast<const ObjectBoolean*>(other);
    return ObjectBoolean::as_object(
        other->get_tag() == OBJECT_BOOLEAN &&
        this->value == boolean->value
    );
} 

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

// ------------------------- ObjectBoolean -------------------------
