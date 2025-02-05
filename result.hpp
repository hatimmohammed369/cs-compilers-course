#ifndef RESULT_H_INCLUDED
#define RESULT_H_INCLUDED

#include <stdexcept>

template <typename T, typename E>
class Result {
public:
    using ValueType = T;
    using ErrorType = E;
protected:
    ValueType value;
    ErrorType error;
    bool _is_ok;
public:
    static Result<ValueType, ErrorType> Ok(ValueType value) {
        return Result{value, {}, true};
    }

    static Result<ValueType, ErrorType> Error(ErrorType error) {
        return Result{{}, error, false};
    }

    inline bool is_ok() const noexcept { return _is_ok; }
    inline bool is_error() const noexcept { return !_is_ok; }

    ValueType unwrap() const {
        if (!_is_ok)
            throw std::runtime_error("Attemping to unwrap a value from an error result");
        return value;
    }

    ErrorType unwrap_error() const {
        if (_is_ok)
            throw std::runtime_error("Attemping to unwrap an error from an value result");
        return error;
    }
};

#endif
