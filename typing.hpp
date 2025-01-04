#ifndef TYPING_H_INCLUDED
#define TYPING_H_INCLUDED

class Type {
public:
    template <typename Target>
    Target* cast(const Target* obj);
};

#endif
