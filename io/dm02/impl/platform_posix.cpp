#include <cstdint>
#include <cstring>

#if defined(_WIN32)
#include <windows.h>
#else
#include <time.h>
#endif

extern "C" {
#include "io/dm02/protocol/core/platform.h"
}

extern "C" uint32_t platform_get_tick_ms(void)
{
#if defined(_WIN32)
  return static_cast<uint32_t>(GetTickCount());
#else
  struct timespec ts {};
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return static_cast<uint32_t>(ts.tv_sec * 1000u + ts.tv_nsec / 1000000u);
#endif
}

extern "C" void platform_delay_ms(uint32_t ms)
{
#if defined(_WIN32)
  Sleep(ms);
#else
  struct timespec req {};
  req.tv_sec = ms / 1000u;
  req.tv_nsec = static_cast<long>((ms % 1000u) * 1000000ul);
  nanosleep(&req, nullptr);
#endif
}

extern "C" void * platform_memcpy(void * dst, const void * src, uint32_t n)
{
  return std::memcpy(dst, src, n);
}

extern "C" void * platform_memset(void * s, int c, uint32_t n)
{
  return std::memset(s, c, n);
}

extern "C" void * platform_memmove(void * dst, const void * src, uint32_t n)
{
  return std::memmove(dst, src, n);
}

extern "C" int platform_memcmp(const void * s1, const void * s2, uint32_t n)
{
  return std::memcmp(s1, s2, n);
}

extern "C" uint32_t platform_strlen(const char * s)
{
  return static_cast<uint32_t>(std::strlen(s ? s : ""));
}

extern "C" char * platform_strcpy(char * dst, const char * src)
{
  return std::strcpy(dst, src ? src : "");
}

extern "C" int platform_strcmp(const char * s1, const char * s2)
{
  return std::strcmp(s1 ? s1 : "", s2 ? s2 : "");
}
