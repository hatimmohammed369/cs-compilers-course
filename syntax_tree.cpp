#include "syntax_tree.hpp"

std::string Void::to_string() const noexcept {
    return Void::get_string_value();
}

template class Literal<i64>;
template class Literal<float64>;

template <typename T>
Literal<T>::Literal(const T& val) {*value_object = val;}

template <typename T>
std::string Literal<T>::to_string() const noexcept {
    return std::to_string(*value_object);
}

template class Literal<bool>;

template <>
std::string Literal<bool>::to_string() const noexcept {
    return std::string(*value_object ? "true" : "false");
}

template class Literal<std::string>;

template <>
std::string Literal<std::string>::to_string() const noexcept {
    std::string repr;
    repr.push_back('"');
    repr.append(*value_object);
    repr.push_back('"');
    return repr;
}

