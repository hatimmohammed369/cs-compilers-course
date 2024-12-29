#ifndef VISITOR_H_INCLUDED
#define VISITOR_H_INCLUDED

class Object;
class Literal;

class Visitor {
public:
    ~Visitor() = default;
    virtual Object* visit_literal(Literal* literal) = 0;
};

#endif
