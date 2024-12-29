#include "object.hpp"

ObjectInteger::ObjectInteger(i64 val): value{val} {}

std::string ObjectInteger::to_string() const noexcept {
    return std::to_string(value);
}
