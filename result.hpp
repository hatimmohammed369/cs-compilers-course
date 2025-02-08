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

    explicit Result() {
        value = {};
        error = {};
        _is_ok = false;
    }

    explicit Result(
        Result::ValueType val,
        Result::ErrorType err,
        bool ok
    ) {
        value = val;
        error = err;
        _is_ok = ok;
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

template <typename PointerType, typename E>
class PointerValueResult:
    public Result<PointerType/*value type*/, E/*error type*/> {
public:
    using Result<PointerType,E>::Result;
    using ValueType = typename Result<PointerType,E>::ValueType;
    using ErrorType = typename Result<PointerType,E>::ErrorType;

    static PointerValueResult Ok(ValueType value) {
        return PointerValueResult{value, {}, true};
    }

    static PointerValueResult Error(ErrorType error) {
        return PointerValueResult{{}, error, false};
    }

    inline bool is_useless() {
        return !this->_is_ok || this->value == nullptr;
    }

    inline bool is_usable() {
        return this->_is_ok && this->value != nullptr;
    }

    inline bool is_null_value() {
        return this->_is_ok && this->value == nullptr;
    }
};

#endif
