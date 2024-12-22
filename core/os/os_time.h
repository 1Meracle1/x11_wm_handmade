#ifndef OS_TIME_H
#define OS_TIME_H

#include "os_defines.h"
#include "../containers/string.h"

#define Nanoseconds(n) (n)
#define Microsecons(n) ((n) * 1000)
#define Milliseconds(n) (Microsecons(n) * 1000)
#define Seconds(n) (Milliseconds(n) * 1000)
#define Minutes(n) (Seconds(n) * 60)
#define Hours(n) (Minutes(n) * 60)
#define Days(n) (Hours(n) * 24)

internal u64 TimeNow();

internal void Sleep(u64 nanos);

internal String StringFromTime(Allocator allocator, u64 nanos);
internal String StringFromTimeRFC3339(Allocator allocator, u64 nanos);

#endif