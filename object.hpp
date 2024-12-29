#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#include "common.hpp"

class Object {
public:
    ~Object() {}
    virtual std::string to_string() const noexcept = 0;
};

class ObjectInteger: public Object {
    i64 value;
public:
    ObjectInteger(i64 val);
    std::string to_string() const noexcept override;
    // More integer specific code here later
};

#endif
