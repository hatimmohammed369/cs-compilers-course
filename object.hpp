#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#include <cstring>
#include "common.hpp"

class Object {
public:
    ~Object() {}
    virtual std::string to_string() const noexcept = 0;
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
    inline T get() const noexcept {return value;}
    virtual std::string to_string() const noexcept = 0;
    virtual Number* operator-() const noexcept = 0;
    Number<i64>* integer_div(const Number<i64>* other) const noexcept;
    Number<i64>* integer_div(const Number<float64>* other) const noexcept;
    Number<float64>* operator/(const Number<i64>* other) const noexcept;
    Number<float64>* operator/(const Number<float64>* other) const noexcept;
};

class ObjectBoolean;
class ObjectFloat;

template class Number<i64>;
class ObjectInteger: public Number<i64> {
public:
    using Number::Number;
    using Number<i64>::operator/;
    std::string to_string() const noexcept override;
    ObjectInteger* operator-() const noexcept override;
    ObjectInteger* operator*(const ObjectInteger* other) const noexcept;
    ObjectFloat* operator*(const ObjectFloat* other) const noexcept;
    ObjectInteger* operator%(const ObjectInteger* other) const noexcept;
    ObjectInteger* operator+(const ObjectInteger* other) const noexcept;
    ObjectFloat* operator+(const ObjectFloat* other) const noexcept;
    ObjectInteger* operator-(const ObjectInteger* other) const noexcept;
    ObjectFloat* operator-(const ObjectFloat* other) const noexcept;
    ObjectBoolean* operator>(const ObjectInteger* other) const noexcept;
    ObjectBoolean* operator>(const ObjectFloat* other) const noexcept;
    // More integer specific code here later
};

template class Number<float64>;
class ObjectFloat: public Number<float64> {
public:
    using Number::Number;
    using Number<float64>::operator/;
    std::string to_string() const noexcept override;
    ObjectFloat* operator-() const noexcept override;
    ObjectFloat* operator*(const ObjectInteger* other) const noexcept;
    ObjectFloat* operator*(const ObjectFloat* other) const noexcept;
    ObjectFloat* operator+(const ObjectInteger* other) const noexcept;
    ObjectFloat* operator+(const ObjectFloat* other) const noexcept;
    ObjectFloat* operator-(const ObjectInteger* other) const noexcept;
    ObjectFloat* operator-(const ObjectFloat* other) const noexcept;
    ObjectBoolean* operator>(const ObjectInteger* other) const noexcept;
    ObjectBoolean* operator>(const ObjectFloat* other) const noexcept;
    // More float specific code here later
};

class ObjectVoid: public Object {
private:
    ObjectVoid() {} // Only a single object availaible
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

    std::string to_string() const noexcept override;
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
    std::string to_string() const noexcept override;
    const char* get() const noexcept;
    size_t length() const noexcept;
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

    std::string to_string() const noexcept override;
    bool get() const noexcept;
};

#endif
