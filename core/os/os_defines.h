#ifndef OS_DEFINES_H
#define OS_DEFINES_H

#include "../core_defines.h"

#if defined(_WIN32) || defined(_WIN64)
#define OS_WINDOWS
#elif defined(__linux__)
#define OS_LINUX
#elif defined(__APPLE__) || defined(__MACH__)
#define OS_MACOS
#elif defined(__unix__) || defined(__unix)
#define OS_UNIX
#elif defined(_POSIX_VERSION)
#define OS_POSIX
#else
#define OS_UNKNOWN
#endif

#endif