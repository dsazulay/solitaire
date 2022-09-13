#pragma once
#include <iostream>

#define DEBUG 1

#define RESET   "\033[0m"
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */

#if DEBUG
#define LOG_ERROR(msg) std::cout << RED << "[ERROR] " << msg << RESET << std::endl
#define LOG_WARN(msg)  std::cout << YELLOW << "[WARN] " << msg << RESET << std::endl
#define LOG_INFO(msg)  std::cout << GREEN << "[INFO] " << msg << RESET << std::endl
#else
#define LOG_ERROR(x)
#define LOG_WARN(x)
#define LOG_INFO(x)
#endif

#if DEBUG
#define ASSERT(exp, msg)                                             \
    if (!(exp))                                                      \
        std::cout << RED << "[ASSERT] " << __FILE__ << ":"           \
        << __LINE__ << " \'" << #exp << "\' failed: " << msg         \
        << RESET << std::endl
#else
#define ASSERT(exp, msg)
#endif
