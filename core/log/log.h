#ifndef LOG_H
#define LOG_H

#include "../core_defines.h"

internal void Logf(const char *prefix, const char *file, int line, const char *format, ...);
internal void Log(const char *prefix, const char *file, int line, const char *msg);

#ifdef DEBUG_BUILD
#define Debugf(format, ...) Logf("\033[90m[DEBUG]\033[0m", __FILE__, __LINE__, format, __VA_ARGS__)
#else
#define Debugf(format, ...) (void)format
#endif
#define Infof(format, ...) Logf("\033[32m[INFO]\033[0m", __FILE__, __LINE__, format, __VA_ARGS__)
#define Warnf(format, ...) Logf("\033[33m[WARN]\033[0m", __FILE__, __LINE__, format, __VA_ARGS__)
#define Errorf(format, ...) Logf("\033[31m[ERROR]\033[0m", __FILE__, __LINE__, format, __VA_ARGS__)

#ifdef DEBUG_BUILD
#define Debug(msg) Log("\033[90m[DEBUG]\033[0m", __FILE__, __LINE__, msg)
#else
#define Debug(msg) (void)msg
#endif
#define Info(msg) Log("\033[32m[INFO]\033[0m", __FILE__, __LINE__, msg)
#define Warn(msg) Log("\033[33m[WARN]\033[0m", __FILE__, __LINE__, msg)
#define Error(msg) Log("\033[31m[ERROR]\033[0m", __FILE__, __LINE__, msg)

#endif