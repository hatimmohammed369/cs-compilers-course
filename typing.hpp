#ifndef TYPING_H_INCLUDED
#define TYPING_H_INCLUDED

#include "object.hpp"
template <ObjectType tag>
class Type {
public:
    template <typename Target>
    Target* cast(const Target* obj);
};

#endif
