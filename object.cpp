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
