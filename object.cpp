#include <iomanip>
#include <sstream>
#include "object.hpp"

template <typename T>
ObjectFloat* Number<T>::operator/(const Number<i64>* other) const noexcept {
    if (!other->get()) {
        std::cerr << "Division by zero\n";
        exit(1);
    }
    return new ObjectFloat {
        static_cast<float64>(get()) / static_cast<float64>(other->get())
    };
}

template <typename T>
ObjectFloat* Number<T>::operator/(const Number<float64>* other) const noexcept {
    if (!other->get()) {
        std::cerr << "Division by zero\n";
        exit(1);
    }
    return new ObjectFloat {
        static_cast<float64>(get()) / static_cast<float64>(other->get())
    };
}

std::string ObjectInteger::to_string() const noexcept {
    return std::to_string(value);
}

ObjectInteger* ObjectInteger::operator-() const noexcept {
    return new ObjectInteger{-value};
}

ObjectInteger* ObjectInteger::operator*(const ObjectInteger* other) const noexcept {
    return new ObjectInteger{value * other->get()};
}

ObjectInteger* ObjectInteger::operator*(const ObjectFloat* other) const noexcept {
    return new ObjectInteger{
        static_cast<i64>(value * other->get())
    };
}

std::string ObjectFloat::to_string() const noexcept {
    std::ostringstream oss;
    oss << std::setprecision(16) ;
    oss << value ;
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

std::string ObjectVoid::to_string() const noexcept {
    return ObjectVoid::get_string_value();
}

ObjectString::ObjectString() {
    chars = nullptr;
    _length = 0;
}

ObjectString::ObjectString(const char* s, size_t len) {
    chars = new char[len + 1];
    chars[len] = '\0';
    strncpy(chars, s, len);
    _length = len;
}

ObjectString::ObjectString(const char* s): ObjectString(s, strlen(s)) {}

ObjectString::ObjectString(const std::string& s):
    ObjectString(s.c_str(), s.length()) {}

ObjectString::~ObjectString() {delete chars;}

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

std::string ObjectBoolean::to_string() const noexcept {
    return str;
}

bool ObjectBoolean::get() const noexcept {
    return value;
}
