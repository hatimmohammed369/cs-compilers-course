#ifndef RESULT_H_INCLUDED
#define RESULT_H_INCLUDED

#include <stdexcept>

template <typename T, typename E>
class Result {
protected:
    T value;
    E error;
    bool _is_ok;
public:
    static Result<T, E> Ok(T value) {
        return Result{value, {}, true};
    }

    static Result<T, E> Error(E error) {
        return Result{{}, error, false};
    }

    inline bool is_ok() const noexcept { return _is_ok; }
    inline bool is_error() const noexcept { return !_is_ok; }

    T unwrap() const {
        if (!_is_ok)
            throw std::runtime_error("Attemping to unwrap a value from an error result");
        return value;
    }

    E unwrap_error() const {
        if (_is_ok)
            throw std::runtime_error("Attemping to unwrap an error from an value result");
        return error;
    }
};

#endif
