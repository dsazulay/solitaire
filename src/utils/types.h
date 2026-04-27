#pragma once

#include <concepts>
#include <cstdint>

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float    f32;
typedef double   f64;

template<typename T>
concept System = requires(T t)
{
    { t.init() } -> std::same_as<void>;
    { t.terminate() } -> std::same_as<void>;
};

template<System T>
auto SystemInit(T& t) -> void
{
    t.init();
}

template<System T>
auto SystemTerminate(T& t) -> void
{
    t.terminate();
}
