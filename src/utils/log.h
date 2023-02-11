#pragma once

#include <fmt/color.h>

#define DEBUG 1

#if DEBUG
#define LOG_ERROR(...) fmt::print(fg(fmt::terminal_color::red), "[ERROR] {}\n", fmt::format( __VA_ARGS__))
#define LOG_WARN(...)  fmt::print(fg(fmt::terminal_color::yellow), "[WARN] {}\n", fmt::format( __VA_ARGS__))
#define LOG_INFO(...)  fmt::print(fg(fmt::terminal_color::green), "[INFO] {}\n", fmt::format( __VA_ARGS__))
#else
#define LOG_ERROR(...)
#define LOG_WARN(...)
#define LOG_INFO(...)
#endif

#if DEBUG
#define ASSERT(exp, msg)  \
    if (!(exp))           \
        fmt::print(fg(fmt::terminal_color::red), "[ASSERT] {}:{} \'{}\' failed: {}\n", __FILE__, __LINE__, #exp, msg)
#else
#define ASSERT(exp, msg)
#endif
