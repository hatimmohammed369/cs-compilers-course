#include "typing.hpp"
#include "token.hpp"

static const char* type_name_string(const TypeName& name) {
    switch (name) {
        case TYPENAME_TYPE:
            return "type";
        case TYPENAME_INT:
            return "int";
        case TYPENAME_FLOAT:
            return "float";
        case TYPENAME_BOOLEAN:
            return "boolean";
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

Type* Type::get_type_by_token(TokenType type_keyword) {
    switch (type_keyword) {
        case TOKEN_KEYWORD_INT:
            return TypeInt::get_int_type_object();
        case TOKEN_KEYWORD_FLOAT:
            return TypeFloat::get_float_type_object();
        case TOKEN_KEYWORD_STRING:
            return TypeString::get_string_type_object();
        case TOKEN_KEYWORD_BOOLEAN:
            return TypeBoolean::get_boolean_type_object();
        case TOKEN_KEYWORD_VOID:
            return TypeVoid::get_void_type_object();
        default: {}
    }
    return nullptr;
}

ObjectInteger* TypeInt::cast(const Object* obj) const noexcept {
    switch (obj->tag) {
        case OBJECT_VOID: {
            return new ObjectInteger{0};
        }
        case OBJECT_FLOAT: {
            const ObjectFloat* _obj =
                reinterpret_cast<const ObjectFloat*>(obj);
            return new ObjectInteger{
                static_cast<i64>(_obj->value)
            };
        }
        case OBJECT_STRING: {
            const ObjectString* _obj =
                reinterpret_cast<const ObjectString*>(obj);
            return new ObjectInteger{
                std::stoll(std::string(_obj->get(), _obj->length()))
            };
        }
        case OBJECT_BOOLEAN: {
            const ObjectBoolean* _obj =
                reinterpret_cast<const ObjectBoolean*>(obj);
            return new ObjectInteger{
                static_cast<i64>(_obj->get())
            };
        }
        case OBJECT_TYPE: {
            // unreachable
            std::cerr << "Type object can't be created by user!\n";
            exit(1);
        }
        default: {
            // case OBJECT_INT
        }
    }
    return reinterpret_cast<ObjectInteger*>(obj->copy());
}
