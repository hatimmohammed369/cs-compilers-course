#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#include "common.hpp"

class Object {
public:
    ~Object() {}
    virtual std::string to_string() const noexcept = 0;
};

#endif
