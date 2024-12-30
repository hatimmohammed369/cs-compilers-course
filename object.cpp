#include <sstream>
#include "object.hpp"

ObjectInteger::ObjectInteger(i64 val): value{val} {}

std::string ObjectInteger::to_string() const noexcept {
    return std::to_string(value);
}

ObjectFloat::ObjectFloat(float64 val): value{val} {}

std::string ObjectFloat::to_string() const noexcept {
    std::ostringstream oss;
    oss << value ;
    return oss.str();
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
