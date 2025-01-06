#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdint>

static std::ostringstream fmt;
static void reset_fmt() {
    fmt.str("");
    fmt.clear();
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
