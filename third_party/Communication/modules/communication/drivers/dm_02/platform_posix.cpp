/**
  * @file        platform_posix.cpp
  * @brief       POSIX platform shim for Communication core library
  * @details     Implements platform.h functions for Linux/macOS.
  */

// platform_posix.cpp (host platform shim for 3rdparty/Communication core)
#include <cstdint>
#include <cstring>

#if defined(_WIN32)
#include <windows.h>
#else
#include <time.h>
#endif

extern "C" {
#include "3rdparty/Communication/core/platform.h"
}

/**
  * @brief          Get system tick count in milliseconds
  * @retval         Milliseconds since boot
  * @details        Uses monotonic clock on POSIX, GetTickCount on Windows.
  */
extern "C" uint32_t platform_get_tick_ms(void) {
#if defined(_WIN32)
    return static_cast<uint32_t>(GetTickCount());
#else
    struct timespec ts{};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<uint32_t>(ts.tv_sec * 1000u + ts.tv_nsec / 1000000u);
#endif
}

/**
  * @brief          Delay execution for specified milliseconds
  * @param[in]      ms: Milliseconds to sleep
  */
extern "C" void platform_delay_ms(uint32_t ms) {
#if defined(_WIN32)
    Sleep(ms);
#else
    struct timespec req{};
    req.tv_sec = ms / 1000u;
    req.tv_nsec = static_cast<long>((ms % 1000u) * 1000000ul);
    nanosleep(&req, nullptr);
#endif
}

/**
  * @brief          Copy memory
  * @param[out]     dst: Destination pointer
  * @param[in]      src: Source pointer
  * @param[in]      n: Number of bytes
  * @retval         Destination pointer
  */
extern "C" void* platform_memcpy(void* dst, const void* src, uint32_t n) {
    return std::memcpy(dst, src, n);
}

/**
  * @brief          Fill memory with constant byte
  * @param[out]     s: Memory pointer
  * @param[in]      c: Fill byte value
  * @param[in]      n: Number of bytes
  * @retval         Memory pointer
  */
extern "C" void* platform_memset(void* s, int c, uint32_t n) {
    return std::memset(s, c, n);
}

/**
  * @brief          Move memory (handles overlapping regions)
  * @param[out]     dst: Destination pointer
  * @param[in]      src: Source pointer
  * @param[in]      n: Number of bytes
  * @retval         Destination pointer
  */
extern "C" void* platform_memmove(void* dst, const void* src, uint32_t n) {
    return std::memmove(dst, src, n);
}

/**
  * @brief          Compare memory
  * @param[in]      s1: First memory region
  * @param[in]      s2: Second memory region
  * @param[in]      n: Number of bytes
  * @retval         <0, 0, or >0 if s1 < s2, s1 == s2, or s1 > s2
  */
extern "C" int platform_memcmp(const void* s1, const void* s2, uint32_t n) {
    return std::memcmp(s1, s2, n);
}

/**
  * @brief          Get string length
  * @param[in]      s: String pointer (treated as "" if null)
  * @retval         String length in bytes
  */
extern "C" uint32_t platform_strlen(const char* s) {
    return static_cast<uint32_t>(std::strlen(s ? s : ""));
}

/**
  * @brief          Copy string
  * @param[out]     dst: Destination buffer
  * @param[in]      src: Source string (treated as "" if null)
  * @retval         Destination pointer
  */
extern "C" char* platform_strcpy(char* dst, const char* src) {
    return std::strcpy(dst, src ? src : "");
}

/**
  * @brief          Compare strings
  * @param[in]      s1: First string (treated as "" if null)
  * @param[in]      s2: Second string (treated as "" if null)
  * @retval         <0, 0, or >0 if s1 < s2, s1 == s2, or s1 > s2
  */
extern "C" int platform_strcmp(const char* s1, const char* s2) {
    return std::strcmp(s1 ? s1 : "", s2 ? s2 : "");
}
