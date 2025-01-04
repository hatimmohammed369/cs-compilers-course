#ifndef TYPING_H_INCLUDED
#define TYPING_H_INCLUDED

#include "object.hpp"
#include "token.hpp"

enum TypeName {
    TYPENAME_TYPE,
    TYPENAME_INT,
    TYPENAME_FLOAT,
    TYPENAME_BOOLEAN,
    TYPENAME_STRING,
    TYPENAME_VOID,
};

class Type: public Object {
protected:
    Type() {tag = OBJECT_TYPE;}
public:
    TypeName name;
    virtual ~Type() = default;
    std::string to_string() const noexcept;
    ObjectBoolean* to_boolean() const noexcept;
    ObjectBoolean* equals(const Object* other) const noexcept;
};

#endif
