#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdint>

static std::ostringstream fmt;
static std::string read_fmt() __attribute__((unused));
static std::string read_fmt() {
    std::string str = fmt.str();
    fmt.str("");
    fmt.clear();
    return str;
}

enum class Mode {
    File,
    Interactive
};

static Mode mode;

static bool is_mode_interactive() __attribute__((unused));
static bool is_mode_interactive() {
    return mode == Mode::Interactive;
}

static bool is_mode_file() __attribute__((unused));
static bool is_mode_file() {
    return mode == Mode::File;
}

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using float64 = long double;

#endif
