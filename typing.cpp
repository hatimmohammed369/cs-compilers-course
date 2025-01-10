#include "typing.hpp"
#include "token.hpp"

const std::string Type::NAME = "type";
const std::string TypeVoid::NAME = "void";
const std::string TypeBoolean::NAME = "boolean";
const std::string TypeString::NAME = "string";
const std::string TypeFloat::NAME = "float";
const std::string TypeInteger::NAME = "int";

ObjectString::ObjectString(): std::string() {
    type_info = TypeString::get_type_object();
}

ObjectString::ObjectString(const char* s): std::string(s) {
    type_info = TypeString::get_type_object();
}

ObjectString::ObjectString(const char* s, size_t len):
    std::string(s, len) {type_info = TypeString::get_type_object();}

ObjectString::ObjectString(const std::string& s):
    std::string(s) {type_info = TypeString::get_type_object();}

ObjectString::ObjectString(const std::string&& s):
    std::string(s) {type_info = TypeString::get_type_object();}

ObjectInteger::ObjectInteger(const i64& value): 
    Number::Number(value) {type_info = TypeInteger::get_type_object();}

ObjectFloat::ObjectFloat(const float64& value): 
    Number::Number(value) {type_info = TypeFloat::get_type_object();}

ObjectVoid::ObjectVoid() {type_info = TypeVoid::get_type_object();}

ObjectBoolean::ObjectBoolean(bool val) {
    type_info = TypeBoolean::get_type_object();
    value = val;
}

ObjectBoolean* ObjectString::equals(const Object* other) const noexcept {
    const ObjectString* str =
        dynamic_cast<const ObjectString*>(other);
    return ObjectBoolean::as_object(
        (str && other->type_info == TypeString::get_type_object()) &&
        *this == *str
    );
}

ObjectBoolean* ObjectBoolean::equals(const Object* other) const noexcept {
    const ObjectBoolean* boolean =
        dynamic_cast<const ObjectBoolean*>(other);
    return ObjectBoolean::as_object(
        (boolean && other->type_info == TypeBoolean::get_type_object()) &&
        this->value == boolean->value
    );
}

ObjectBoolean* ObjectVoid::equals(const Object* other) const noexcept {
    // There is only one (void)
    return ObjectBoolean::as_object(
        other->type_info == TypeVoid::get_type_object()
    );
}

Object* Type::copy() const noexcept {
    return this->type_info;
}

ObjectBoolean* Type::equals(const Object* other) const noexcept {
    const Type* type_obj =
        dynamic_cast<const Type*>(other);
    return ObjectBoolean::as_object(
        (type_obj && type_obj->type_info == Type::get_type_object()) &&
        type_obj->type_name == this->type_name
    );
}

std::string Type::to_string() const noexcept {
    fmt << "<type '" ;
    fmt << this->type_name ;
    fmt << "'>" ;
    return read_fmt();
}

ObjectBoolean* Type::to_boolean() const noexcept {
    return ObjectBoolean::TRUE;
}

Object* Type::cast(const Object* obj) const noexcept {
    return obj->type_info;
}

Type* Type::get_type_by_token(TokenType type_keyword) {
    switch (type_keyword) {
        case TokenType::KEYWORD_TYPE:
            return Type::get_type_object();
        case TokenType::KEYWORD_INT:
            return TypeInteger::get_type_object();
        case TokenType::KEYWORD_FLOAT:
            return TypeFloat::get_type_object();
        case TokenType::KEYWORD_STRING:
            return TypeString::get_type_object();
        case TokenType::KEYWORD_BOOLEAN:
            return TypeBoolean::get_type_object();
        case TokenType::KEYWORD_VOID:
            return TypeVoid::get_type_object();
        default: {}
    }
    return nullptr;
}

ObjectInteger* TypeInteger::cast(const Object* obj) const noexcept {
    const std::string& obj_type_name = obj->type_info->type_name;
    if (obj_type_name == TypeVoid::NAME) {
        return new ObjectInteger{0};
    } else if (obj_type_name == TypeFloat::NAME) {
        const ObjectFloat* _obj =
            reinterpret_cast<const ObjectFloat*>(obj);
        return new ObjectInteger{
            static_cast<i64>(_obj->value)
        };
    } else if (obj_type_name == TypeString::NAME) {
        const ObjectString* _obj =
            reinterpret_cast<const ObjectString*>(obj);
        return new ObjectInteger{
            std::stoll(std::string(_obj->c_str(), _obj->size()))
        };
    } else if (obj_type_name == TypeBoolean::NAME) {
        const ObjectBoolean* _obj =
            reinterpret_cast<const ObjectBoolean*>(obj);
        return new ObjectInteger{
            static_cast<i64>(_obj->get())
        };
    } else if (obj_type_name == Type::NAME) {
        // unreachable
        std::cerr << "Type object can't be casted to `int`\n";
        exit(1);
    }
    return reinterpret_cast<ObjectInteger*>(obj->copy());
}

ObjectFloat* TypeFloat::cast(const Object* obj) const noexcept {
    const std::string& obj_type_name = obj->type_info->type_name;
    if (obj_type_name == TypeVoid::NAME) {
        return new ObjectFloat{0.0};
    } else if (obj_type_name == TypeInteger::NAME) {
        const ObjectInteger* _obj =
            reinterpret_cast<const ObjectInteger*>(obj);
        return new ObjectFloat{
            static_cast<float64>(_obj->value)
        };
    } else if (obj_type_name == TypeString::NAME) {
        const ObjectString* _obj =
            reinterpret_cast<const ObjectString*>(obj);
        return new ObjectFloat{
            std::stold(std::string(_obj->c_str(), _obj->size()), nullptr)
        };
    } else if (obj_type_name == TypeBoolean::NAME) {
        const ObjectBoolean* _obj =
            reinterpret_cast<const ObjectBoolean*>(obj);
        return new ObjectFloat{
            static_cast<float64>(_obj->get())
        };
    } else if (obj_type_name == Type::NAME) {
        // unreachable
        std::cerr << "Type object can't be casted to `float`\n";
        exit(1);
    }
    return reinterpret_cast<ObjectFloat*>(obj->copy());
}

ObjectString* TypeString::cast(const Object* obj) const noexcept {
    return new ObjectString(obj->to_string());
}

ObjectBoolean* TypeBoolean::cast(const Object* obj) const noexcept {
    return obj->to_boolean();
}

ObjectVoid* TypeVoid::cast(const Object* obj) const noexcept {
    return ObjectVoid::VOID_OBJECT;
}
