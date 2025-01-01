#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#include <cstring>
#include "common.hpp"

enum ObjectType {
    OBJECT_VOID,
    OBJECT_BOOLEAN,
    OBJECT_INTEGER,
    OBJECT_FLOAT,
    OBJECT_STRING,
};

class ObjectBoolean;
class ObjectFloat;

class Object {
protected:
    ObjectType tag;
public:
    friend class Interpreter;
    ~Object() {}
    virtual std::string to_string() const noexcept = 0;
    virtual ObjectBoolean* equals(const Object* other) const noexcept = 0;
    inline ObjectType get_tag() const noexcept {return tag;}
    virtual ObjectBoolean* to_boolean() const noexcept = 0;
};

inline std::ostream& operator<<(std::ostream& os, const Object* obj) {
    return os << obj->to_string() ;
}

template <typename T>
class Number: public Object {
protected:
    T value;
public:
    friend class Interpreter;
    Number(const T& val): value{val} {}

    ObjectBoolean* equals(const Object* other) const noexcept override;
    inline T get() const noexcept {return value;}

    virtual Number* operator-() const noexcept = 0;

    Number<i64>* integer_div(const Number<i64>* other) const noexcept;
    Number<i64>* integer_div(const Number<float64>* other) const noexcept;
    Number<float64>* operator/(const Number<i64>* other) const noexcept;
    Number<float64>* operator/(const Number<float64>* other) const noexcept;

    template <typename U>
    ObjectBoolean* operator>(const Number<U>* other) const noexcept;
    template <typename U>
    ObjectBoolean* operator>=(const Number<U>* other) const noexcept;
    template <typename U>
    ObjectBoolean* operator<(const Number<U>* other) const noexcept;
    template <typename U>
    ObjectBoolean* operator<=(const Number<U>* other) const noexcept;

    ObjectBoolean* to_boolean() const noexcept override;
};

template class Number<i64>;
template class Number<float64>;

class ObjectInteger: public Number<i64> {
public:
    ObjectInteger(const i64& val):
        Number(val) {tag = OBJECT_INTEGER;}
    std::string to_string() const noexcept override;
    ObjectInteger* operator-() const noexcept override;
    ObjectInteger* operator*(const ObjectInteger* other) const noexcept;
    ObjectFloat* operator*(const ObjectFloat* other) const noexcept;
    ObjectInteger* operator%(const ObjectInteger* other) const noexcept;
    ObjectInteger* operator+(const ObjectInteger* other) const noexcept;
    ObjectFloat* operator+(const ObjectFloat* other) const noexcept;
    ObjectInteger* operator-(const ObjectInteger* other) const noexcept;
    ObjectFloat* operator-(const ObjectFloat* other) const noexcept;
    ObjectInteger* operator~() const noexcept;
    ObjectInteger* operator>>(const ObjectInteger* count) const noexcept;
    ObjectInteger* operator<<(const ObjectInteger* count) const noexcept;
    ObjectInteger* operator&(const ObjectInteger* count) const noexcept;
    ObjectInteger* operator^(const ObjectInteger* count) const noexcept;
    ObjectInteger* operator|(const ObjectInteger* count) const noexcept;
    // More integer specific code here later
};

class ObjectFloat: public Number<float64> {
public:
    ObjectFloat(const float64& val):
        Number(val) {tag = OBJECT_FLOAT;}
    std::string to_string() const noexcept override;
    ObjectFloat* operator-() const noexcept override;
    ObjectFloat* operator*(const ObjectInteger* other) const noexcept;
    ObjectFloat* operator*(const ObjectFloat* other) const noexcept;
    ObjectFloat* operator+(const ObjectInteger* other) const noexcept;
    ObjectFloat* operator+(const ObjectFloat* other) const noexcept;
    ObjectFloat* operator-(const ObjectInteger* other) const noexcept;
    ObjectFloat* operator-(const ObjectFloat* other) const noexcept;
    // More float specific code here later
};

class ObjectVoid: public Object {
private:
    ObjectVoid() {tag = OBJECT_VOID;} // Only a single object availaible
    ObjectVoid(const ObjectVoid&) = delete; // No copy constructor
    ObjectVoid& operator=(const ObjectVoid&) = delete; // No copy assignment

    static const std::string& get_string_value() {
        static std::string value = std::string("void");
        return value;
    }
public:
    static ObjectVoid* get_void_object() {
        static ObjectVoid* void_object = new ObjectVoid{};
        return void_object;
    }

    ObjectBoolean* equals(const Object* other) const noexcept override;
    std::string to_string() const noexcept override;
    ObjectBoolean* to_boolean() const noexcept override;
};

class ObjectString: public Object {
    char* chars;
    size_t _length;
public:
    ObjectString();
    ObjectString(const char* s, size_t len);
    ObjectString(const char* s);
    ObjectString(const std::string& s);
    ~ObjectString();

    ObjectBoolean* equals(const Object* other) const noexcept override;
    std::string to_string() const noexcept override;
    const char* get() const noexcept;
    size_t length() const noexcept;

    ObjectBoolean* to_boolean() const noexcept override;
    // More string specific code here later
};

class ObjectBoolean: public Object {
    bool value;
    std::string str;
    ObjectBoolean(bool val) {
        value = val;
        str = std::string(val ? "true" : "false");
    }
public:
    static ObjectBoolean* get_true_object() {
        static ObjectBoolean* true_obj =
            new ObjectBoolean{true};
        return true_obj;
    }

    static ObjectBoolean* get_false_object() {
        static ObjectBoolean* false_obj =
            new ObjectBoolean{false};
        return false_obj;
    }

    static ObjectBoolean* as_object(bool value) {
        return (value ? get_true_object() : get_false_object());
    }

    ObjectBoolean* equals(const Object* other) const noexcept override;
    std::string to_string() const noexcept override;
    bool get() const noexcept;

    ObjectBoolean* negated() const noexcept;
    ObjectBoolean* operator!() const noexcept;

    ObjectBoolean* operator&&(const ObjectBoolean* other) const noexcept;

    ObjectBoolean* to_boolean() const noexcept override;
};

template <typename T>
template <typename U>
ObjectBoolean* Number<T>::operator>(const Number<U>* other) const noexcept {
    return ObjectBoolean::as_object(value > other->get());
}

template <typename T>
template <typename U>
ObjectBoolean* Number<T>::operator>=(const Number<U>* other) const noexcept {
    return ObjectBoolean::as_object(value >= other->get());
}

template <typename T>
template <typename U>
ObjectBoolean* Number<T>::operator<(const Number<U>* other) const noexcept {
    return ObjectBoolean::as_object(value < other->get());
}

template <typename T>
template <typename U>
ObjectBoolean* Number<T>::operator<=(const Number<U>* other) const noexcept {
    return ObjectBoolean::as_object(value <= other->get());
}

#endif
