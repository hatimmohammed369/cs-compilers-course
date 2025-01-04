#include <sstream>
#include "typing.hpp"

std::string Type::to_string() const noexcept {
    std::ostringstream oss;
    oss << "<type '" ;
    oss << type_name_string(name);
    oss << "'>" ;
    return std::string();
}

ObjectBoolean* Type::to_boolean() const noexcept {
    return ObjectBoolean::get_true_object();
}
