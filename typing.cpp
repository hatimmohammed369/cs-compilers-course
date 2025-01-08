#include "typing.hpp"
#include "token.hpp"

Type* Type::type_type_object = new Type;
const std::string Type::NAME = "type";

TypeVoid* TypeVoid::void_type_object = new TypeVoid;
const std::string TypeVoid::NAME = "void";

TypeBoolean* TypeBoolean::boolean_type_object = new TypeBoolean;
const std::string TypeBoolean::NAME = "boolean";

TypeString* TypeString::string_type_object = new TypeString;
const std::string TypeString::NAME = "string";

TypeFloat* TypeFloat::float_type_object = new TypeFloat;
const std::string TypeFloat::NAME = "float";

TypeInteger* TypeInteger::int_type_object = new TypeInteger;
const std::string TypeInteger::NAME = "int";


ObjectInteger::ObjectInteger(const i64& value): 
    Number::Number(value) {type_info = TypeInteger::int_type_object;}

ObjectFloat::ObjectFloat(const float64& value): 
    Number::Number(value) {type_info = TypeFloat::float_type_object;}

ObjectVoid::ObjectVoid() {type_info = TypeVoid::void_type_object;}

ObjectBoolean::ObjectBoolean(bool val) {
    type_info = TypeBoolean::boolean_type_object;
    value = val;
    str = std::string(val ? "true" : "false");
}

ObjectString::ObjectString() {
    type_info = TypeString::string_type_object;
    chars = nullptr;
    _length = 0;
}

ObjectString::ObjectString(const char* s, size_t len) {
    type_info = TypeString::string_type_object;
    chars = new char[len + 1];
    chars[len] = '\0';
    std::strncpy(chars, s, len);
    _length = len;
}

ObjectBoolean* ObjectString::equals(const Object* other) const noexcept {
    const ObjectString* str =
        dynamic_cast<const ObjectString*>(other);
    return ObjectBoolean::as_object(
        other->type_info == TypeString::string_type_object &&
        this->_length == str->_length &&
        std::strncmp(this->chars, str->chars, this->_length) == 0
    );
}

ObjectBoolean* ObjectBoolean::equals(const Object* other) const noexcept {
    const ObjectBoolean* boolean =
        dynamic_cast<const ObjectBoolean*>(other);
    return ObjectBoolean::as_object(
        other->type_info == TypeBoolean::boolean_type_object &&
        this->value == boolean->value
    );
}

ObjectBoolean* ObjectVoid::equals(const Object* other) const noexcept {
    // There is only one (void)
    return ObjectBoolean::as_object(
        other->type_info == TypeBoolean::boolean_type_object
    );
}

Object* Type::copy() const noexcept {
    return this->type_info;
}

ObjectBoolean* Type::equals(const Object* other) const noexcept {
    const Type* type_obj =
        dynamic_cast<const Type*>(other);
    return ObjectBoolean::as_object(
        type_obj && type_obj->type_info == this->type_info
    );
}

std::string Type::to_string() const noexcept {
    fmt << "<type '"
        << this->type_name
        << "'>";
    return read_fmt();
}

ObjectBoolean* Type::to_boolean() const noexcept {
    return ObjectBoolean::get_true_object();
}

Object* Type::cast(const Object* obj) const noexcept {
    return obj->type_info;
}

Type* Type::get_type_by_token(TokenType type_keyword) {
    switch (type_keyword) {
        case TokenType::KEYWORD_INT:
            return TypeInteger::int_type_object;
        case TokenType::KEYWORD_FLOAT:
            return TypeFloat::float_type_object;
        case TokenType::KEYWORD_STRING:
            return TypeString::string_type_object;
        case TokenType::KEYWORD_BOOLEAN:
            return TypeBoolean::boolean_type_object;
        case TokenType::KEYWORD_VOID:
            return TypeVoid::void_type_object;
        default: {}
    }
    return nullptr;
}

ObjectInteger* TypeInteger::cast(const Object* obj) const noexcept {
        if (obj->type_info->type_name == TypeVoid::NAME) {
            return new ObjectInteger{0};
        } else if (obj->type_info->type_name == TypeFloat::NAME) {
            const ObjectFloat* _obj =
                reinterpret_cast<const ObjectFloat*>(obj);
            return new ObjectInteger{
                static_cast<i64>(_obj->value)
            };
        } else if (obj->type_info->type_name == TypeString::NAME) {
            const ObjectString* _obj =
                reinterpret_cast<const ObjectString*>(obj);
            return new ObjectInteger{
                std::stoll(std::string(_obj->get(), _obj->length()))
            };
        } else if (obj->type_info->type_name == TypeBoolean::NAME) {
            const ObjectBoolean* _obj =
                reinterpret_cast<const ObjectBoolean*>(obj);
            return new ObjectInteger{
                static_cast<i64>(_obj->get())
            };
        } else if (obj->type_info->type_name == Type::NAME) {
            // unreachable
            std::cerr << "Type object can't be created by user!\n";
            exit(1);
        }
        else {
            // case OBJECT_INT
        }
    return reinterpret_cast<ObjectInteger*>(obj->copy());
}

ObjectFloat* TypeFloat::cast(const Object* obj) const noexcept {
        if (obj->type_info->type_name == TypeVoid::NAME) {
            return new ObjectFloat{0.0};
        } else if (obj->type_info->type_name == TypeInteger::NAME) {
            const ObjectInteger* _obj =
                reinterpret_cast<const ObjectInteger*>(obj);
            return new ObjectFloat{
                static_cast<float64>(_obj->value)
            };
        } else if (obj->type_info->type_name == TypeString::NAME) {
            const ObjectString* _obj =
                reinterpret_cast<const ObjectString*>(obj);
            return new ObjectFloat{
                std::stold(std::string(_obj->get(), _obj->length()), nullptr)
            };
        } else if (obj->type_info->type_name == TypeBoolean::NAME) {
            const ObjectBoolean* _obj =
                reinterpret_cast<const ObjectBoolean*>(obj);
            return new ObjectFloat{
                static_cast<float64>(_obj->get())
            };
        } else if (obj->type_info->type_name == Type::NAME) {
            // unreachable
            std::cerr << "Type object can't be created by user!\n";
            exit(1);
        }
        else {
            // case OBJECT_FLOAT
        }
    return reinterpret_cast<ObjectFloat*>(obj->copy());
}

ObjectString* TypeString::cast(const Object* obj) const noexcept {
        if (obj->type_info->type_name == TypeVoid::NAME) {
            return new ObjectString{"void"};
        } else if (obj->type_info->type_name == TypeInteger::NAME) {
            const ObjectInteger* _obj =
                reinterpret_cast<const ObjectInteger*>(obj);
            return new ObjectString{std::to_string(_obj->get())};
        } else if (obj->type_info->type_name == TypeFloat::NAME) {
            const ObjectFloat* _obj =
                reinterpret_cast<const ObjectFloat*>(obj);
            return new ObjectString{std::to_string(_obj->get())};
        } else if (obj->type_info->type_name == TypeBoolean::NAME) {
            const ObjectBoolean* _obj =
                reinterpret_cast<const ObjectBoolean*>(obj);
            return new ObjectString{
                _obj->get() ? "true" : "false"
            };
        } else if (obj->type_info->type_name == Type::NAME) {
            // unreachable
            std::cerr << "Type object can't be created by user!\n";
            exit(1);
        }
        else {
            // case OBJECT_STRING
        }
    return reinterpret_cast<ObjectString*>(obj->copy());
}

ObjectBoolean* TypeBoolean::cast(const Object* obj) const noexcept {
    return obj->to_boolean();
}

ObjectVoid* TypeVoid::cast(const Object* obj) const noexcept {
    return ObjectVoid::get_void_object();
}
