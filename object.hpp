#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#include "common.hpp"

class Type;
class ObjectBoolean;
class ObjectFloat;

class Object {
public:
    Type* type_info;
    virtual ~Object() = default;
    virtual Object* copy() const noexcept = 0;
    virtual ObjectBoolean* equals(const Object* other) const noexcept = 0;
    virtual std::string to_string() const noexcept = 0;
    virtual ObjectBoolean* to_boolean() const noexcept = 0;
};

inline std::ostream& operator<<(std::ostream& os, const Object* obj) {
    return os << obj->to_string() ;
}

template <typename T>
class Number: public Object {
public:
    T value;
    Number(const T& val): value{val} {}

    ObjectBoolean* equals(const Object* other) const noexcept override;
    inline T get() const noexcept {return value;}

    virtual Number* operator-() const noexcept = 0;

    template <typename U>
    Number<i64>* integer_div(const Number<U>* other) const noexcept;
    template <typename U>
    Number<float64>* operator/(const Number<U>* other) const noexcept;

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
    ObjectInteger(const i64& val);
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
    ObjectInteger* copy() const noexcept override;
    // More integer specific code here later
};

class ObjectFloat: public Number<float64> {
public:
    ObjectFloat(const float64& val);
    std::string to_string() const noexcept override;
    ObjectFloat* operator-() const noexcept override;
    ObjectFloat* operator*(const ObjectInteger* other) const noexcept;
    ObjectFloat* operator*(const ObjectFloat* other) const noexcept;
    ObjectFloat* operator+(const ObjectInteger* other) const noexcept;
    ObjectFloat* operator+(const ObjectFloat* other) const noexcept;
    ObjectFloat* operator-(const ObjectInteger* other) const noexcept;
    ObjectFloat* operator-(const ObjectFloat* other) const noexcept;
    ObjectFloat* copy() const noexcept override;
    // More float specific code here later
};

class ObjectVoid: public Object {
private:
    ObjectVoid(); // Only a single object availaible
    ObjectVoid(const ObjectVoid&) = delete; // No copy constructor
    ObjectVoid& operator=(const ObjectVoid&) = delete; // No copy assignment
    ObjectVoid(const ObjectVoid&&) = delete; // No move constructor
    ObjectVoid& operator=(const ObjectVoid&&) = delete; // No move assignment
public:
    static const std::string NAME;
    static ObjectVoid* VOID_OBJECT;

    ObjectBoolean* equals(const Object* other) const noexcept override;
    std::string to_string() const noexcept override;
    ObjectBoolean* to_boolean() const noexcept override;
    ObjectVoid* copy() const noexcept override;
};

class ObjectString: public Object, public std::string {
public:
    ObjectString();
    ObjectString(const char* s);
    ObjectString(const char* s, size_t len);
    ObjectString(const std::string& s);
    ObjectString(const std::string&& s);

    ObjectBoolean* equals(const Object* other) const noexcept override;
    std::string to_string() const noexcept override;

    ObjectBoolean* to_boolean() const noexcept override;
    ObjectString* copy() const noexcept override;
    // More string specific code here later
};

class ObjectBoolean: public Object {
private:
    ObjectBoolean(const ObjectBoolean&) = delete; // No copy constructor
    ObjectBoolean& operator=(const ObjectBoolean&) = delete; // No copy assignment
    ObjectBoolean(const ObjectBoolean&&) = delete; // No move constructor
    ObjectBoolean& operator=(const ObjectBoolean&&) = delete; // No move assignment

    bool value;
    std::string str;
    ObjectBoolean(bool val);
public:
    static ObjectBoolean* TRUE;
    static ObjectBoolean* FALSE;

    static ObjectBoolean* as_object(bool value) {
        return (value ? TRUE : FALSE);
    }

    ObjectBoolean* equals(const Object* other) const noexcept override;
    std::string to_string() const noexcept override;
    bool get() const noexcept;

    ObjectBoolean* negated() const noexcept;
    ObjectBoolean* operator!() const noexcept;

    ObjectBoolean* operator&&(const ObjectBoolean* other) const noexcept;
    ObjectBoolean* operator||(const ObjectBoolean* other) const noexcept;
    ObjectBoolean* xor_with(const ObjectBoolean* other) const noexcept;

    ObjectBoolean* to_boolean() const noexcept override;
    ObjectBoolean* copy() const noexcept override;
};

// ------------------------- Number -------------------------

template <typename T>
ObjectBoolean* Number<T>::to_boolean() const noexcept {
    // Zero is false
    // Non-zero is true
    return ObjectBoolean::as_object(this->value != 0);
}

template <typename T>
template <typename U>
Number<i64>* Number<T>::integer_div(const Number<U>* other) const noexcept {
    if (!other->get()) {
        std::cerr << "Division by zero\n";
        exit(1);
    }
    return new ObjectInteger {
        static_cast<i64>(get()) / static_cast<i64>(other->get())
    };
}

template <typename T>
template <typename U>
Number<float64>* Number<T>::operator/(const Number<U>* other) const noexcept {
    if (!other->get()) {
        std::cerr << "Division by zero\n";
        exit(1);
    }
    return new ObjectFloat {
        static_cast<float64>(get()) / static_cast<float64>(other->get())
    };
}

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

// ------------------------- Number -------------------------

#endif
