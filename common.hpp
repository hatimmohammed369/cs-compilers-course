#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <cstring>
#include <cstdint>
#include <format>
#include <iostream>
#include <print>
#include <sstream>
#include <string>
#include <vector>

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

class Config {
public:
    static Mode* get_mode() {
        static Mode* m = new Mode;
        return m;
    }
};

static bool is_mode_interactive() __attribute__((unused));
static bool is_mode_interactive() {
    return *Config::get_mode() == Mode::Interactive;
}

static bool is_mode_file() __attribute__((unused));
static bool is_mode_file() {
    return *Config::get_mode() == Mode::File;
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
