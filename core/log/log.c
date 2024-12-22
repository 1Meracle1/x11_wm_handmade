#include "log.h"
#include <time.h>
#include <stdarg.h>

internal void Logf(const char *prefix, const char *file, int line, const char *format, ...)
{
  time_t now;
  time(&now);
  struct tm *tm_info = localtime(&now);

  char timestamp[70];
  strftime(timestamp, sizeof timestamp, "[%Y-%m-%d %H:%M:%S]", tm_info);

  printf("%s %s (%s:%d): ", prefix, timestamp, file, line);

  // Initialize the va_list for variadic arguments
  va_list args;
  va_start(args, format);

  // Forward the format and variadic arguments to printf
  vprintf(format, args);

  // Clean up the va_list
  va_end(args);

  printf("\n");
}

internal void Log(const char *prefix, const char *file, int line, const char* msg)
{
  time_t now;
  time(&now);
  struct tm *tm_info = localtime(&now);

  char timestamp[70];
  strftime(timestamp, sizeof timestamp, "[%Y-%m-%d %H:%M:%S]", tm_info);

  printf("%s %s (%s:%d): %s\n", prefix, timestamp, file, line, msg);
}