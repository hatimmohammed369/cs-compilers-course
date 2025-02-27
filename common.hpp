#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <cstring>
#include <cstdint>
#include <format>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

static const std::set<std::string> KEYWORDS {
    "and",
    "boolean",
    "false",
    "float",
    "int",
    "or",
    "print",
    "return",
    "string",
    "true",
    "type",
    "void",
    "xor",
};

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

    static std::string* get_filename() {
        static std::string* filename = new std::string{};
        return filename;
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
