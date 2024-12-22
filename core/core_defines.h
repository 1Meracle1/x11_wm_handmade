#ifndef CORE_DEFINES_H
#define CORE_DEFINES_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#if !defined(DEBUG_BUILD) && !defined(RELEASE_BUILD) && !defined(TEST_BUILD)
#warning                                                                                           \
    "Neither of DEBUG_BUILD, RELEASE_BUILD, or TEST_BUILD is defined, defaulting to DEBUG_BUILD"
#define DEBUG_BUILD
#endif

#define internal [[maybe_unused]] static

#define Bytes(n) (n)
#define Kilobytes(n) (n << 10)
#define Megabytes(n) (n << 20)
#define Gigabytes(n) (((u64)n) << 30)
#define Terabytes(n) (((u64)n) << 40)

#define Thousand(n) ((n) * 1000)
#define Million(n) ((n) * 1000000)
#define Billion(n) ((n) * 1000000000LL)

#define Min(a, b) (((a) < (b)) ? (a) : (b))
#define Max(a, b) (((a) > (b)) ? (a) : (b))

#define ClampTop(x, a) Min(x, a)
#define ClampBot(a, x) Max(a, x)
#define Clamp(a, x, b) (((a) > (x)) ? (a) : ((b) < (x)) ? (b) : (x))

#define SwapT(a, b, T)                                                                             \
  do                                                                                               \
  {                                                                                                \
    T _swap_temp_##T = a;                                                                          \
    a                = b;                                                                          \
    b                = _swap_temp_##T;                                                             \
  } while (0)
#define SwapMem(a, b)                                                                              \
  do                                                                                               \
  {                                                                                                \
    Assert(sizeof(a) == sizeof(b));                                                                \
    u8 _swapmem_temp_[sizeof(a)];                                                                  \
    memcpy(_swapmem_temp_, &a, sizeof(a));                                                         \
    memcpy(&a, &b, sizeof(a));                                                                     \
    memcpy(&b, _swapmem_temp_, sizeof(b));                                                         \
  } while (0)

#ifdef DEBUG_BUILD
#define Assert(condition)                                                                          \
  do                                                                                               \
  {                                                                                                \
    if (!(condition))                                                                              \
    {                                                                                              \
      fprintf(stderr, "%s:%d: assertion failed: %s\n", __FILE__, __LINE__, #condition);            \
      __builtin_trap();                                                                            \
    }                                                                                              \
  } while (0)
#else
#define Assert(condition) (void)(condition)
#endif

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef i8       b8;
typedef i16      b16;
typedef i32      b32;
typedef i64      b64;
typedef float    f32;
typedef double   f64;

#endif