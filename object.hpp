#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#include <cstring>
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

class ObjectFloat: public Object {
    float64 value;
public:
    ObjectFloat(float64 val);
    std::string to_string() const noexcept override;
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
    size_t length;
public:
    ObjectString();
    ObjectString(const char* s, size_t len);
    ObjectString(const char* s);
    ObjectString(const std::string& s);
    ~ObjectString();
    std::string to_string() const noexcept override;
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

    std::string to_string() const noexcept override;
};

#endif
