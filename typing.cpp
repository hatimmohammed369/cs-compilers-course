#include <sstream>
#include "typing.hpp"

static const char* type_name_string(const TypeName& name) {
    switch (name) {
        case TYPENAME_TYPE:
            return "type";
        case TYPENAME_INT:
            return "int";
        case TYPENAME_FLOAT:
            return "float";
        case TYPENAME_BOOL:
            return "bool";
        case TYPENAME_STRING:
            return "string";
        case TYPENAME_VOID:
            return "void";
        default: {}
    }
    return "MISSING_TYPE_NAME";
}

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

ObjectBoolean* Type::equals(const Object* other) const noexcept {
    const Type* type_obj =
        dynamic_cast<const Type*>(other);
    return ObjectBoolean::as_object(
        other->tag == OBJECT_TYPE &&
        this->name == type_obj->name
    );
}
