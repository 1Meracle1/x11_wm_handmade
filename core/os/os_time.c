#include "os_time.h"
#include <time.h>

#ifdef OS_LINUX
#include <unistd.h>

internal u64 TimeNow()
{
  u64             nanos = 0;
  struct timespec ts;
  if (clock_gettime(CLOCK_REALTIME, &ts) == 0)
  {
    nanos = ts.tv_sec * Seconds(1) + ts.tv_nsec;
  }
  return nanos;
}

internal void Sleep(u64 nanos)
{
  if (nanos != 0)
  {
    struct timespec req, rem;
    req.tv_sec  = nanos / Seconds(1);
    req.tv_nsec = nanos % Seconds(1);
    nanosleep(&req, &rem);
  }
}

internal String StringFromTime(Allocator allocator, u64 nanos)
{
  String res = {0};

  time_t     t       = (time_t)nanos / Seconds(1);
  struct tm *tm_info = localtime(&t);
  Assert(tm_info != NULL);
  char timestamp[70];
  strftime(timestamp, sizeof timestamp, "%Y-%m-%d %H:%M:%S", tm_info);

  u64 str_len = strlen(timestamp);
  res.data    = Alloc(u8, str_len);
  res.size    = str_len;
  memcpy(res.data, timestamp, str_len);

  return res;
}

internal String StringFromTimeRFC3339(Allocator allocator, u64 nanos)
{
  String res = {0};

  time_t     t       = (time_t)nanos / Seconds(1);
  struct tm *tm_info = localtime(&t);
  Assert(tm_info != NULL);
  char timestamp[70];
  strftime(timestamp, sizeof timestamp, "%Y-%m-%dT%H:%M:%SZ", tm_info);

  u64 str_len = strlen(timestamp);
  res.data    = Alloc(u8, str_len);
  res.size    = str_len;
  memcpy(res.data, timestamp, str_len);

  return res;
}

#else
#error Time OS interface is not implemented on this platform
#endif