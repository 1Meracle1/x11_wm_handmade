#ifndef STRING_H
#define STRING_H

#include "../core_defines.h"
#include "../memory/memory.h"
#include "array.h"

typedef struct String
{
  u8 *data;
  u64 size;
} String;

#define Str(data, size)                                                                            \
  (String)                                                                                         \
  {                                                                                                \
    (u8 *)(data), (u64)(size)                                                                      \
  }
#define StrLit(lit) Str((u8 *)(lit), sizeof(lit) - 1)
#define StrCstr(cstr) Str((u8 *)(cstr), strlen(cstr))

#define StrFmtTempl "%.*s"
#define StrFmtVal(s) (int)s.size, s.data

internal char  *CstrFromStr(Allocator allocator, String s);
internal String StrClone(Allocator allocator, String s);
internal String StrFromCstrClone(Allocator allocator, char *cstr, u64 size);

internal bool StrEquals(String lhs, String rhs);

internal bool StrIsEmpty(String s);

internal String StrSubstr(String s, u64 start, u64 end);
internal String StrSubstrFrom(String s, u64 start);
internal String StrSubstrTill(String s, u64 end);

internal u64 StrFindSubStr(String haystack, String needle, u64 min);

internal String StrTrimLeft(String s, String trimmed_chars);
internal String StrTrimRight(String s, String trimmed_chars);
internal String StrTrim(String s, String trimmed_chars);
internal String StrTrimSpaces(String s);

internal String StrJoin(Allocator allocator, String lhs, String sep, String rhs);

/*
Returns -1 on failure to find the provided byte
*/
internal i64 StrIndexByte(String s, u8 byte);

typedef enum
{
  StrParseError_None,
  StrParseError_EmptyString,
  StrParseError_InvalidNumberString,
} StrParseError;

/*
Example:
    String s     = StrLit("123");
    u64    value = 0;
    Assert(U64FromStr(s, &value) == StrParseError_None);
    Assert(value == 123);
*/
internal StrParseError U64FromStr(String s, u64 *value);

/*
Example:
    String s     = StrLit("123.2");
    f64    value = 0;
    Assert(F64FromStr(s, &value) == StrParseError_None);
    Debugf("%f", value);
*/
internal StrParseError F64FromStr(String s, f64 *value);

internal String StringFromU64(Allocator allocator, u64 value);
internal String StringFromF64(Allocator allocator, f64 value, u64 precision);

ArrayTemplate(String);

internal String       Array_String_Join(Allocator allocator, Array_String arr, String sep);
internal Array_String StrSplit(Allocator allocator, String s, String sep);
// internal Array_String StrSplitMulti(Allocator allocator, String s, Array_String substrs);
// internal Array_String StrSplitLines(Allocator allocator, String s);

// Owns its data
typedef struct
{
  Array_String data;
} StrBuilder;

internal StrBuilder StrBuilder_Init(Allocator allocator, u64 min_capacity);
internal void       StrBuilder_Deinit(Allocator allocator, StrBuilder *builder);
internal String     StrBuilder_ToString(Allocator allocator, StrBuilder builder);
internal void       StrBuilder_PushStr(Allocator allocator, StrBuilder *builder, String s);
internal void       StrBuilder_PushCstr(Allocator allocator, StrBuilder *builder, char *cstr);
internal void       StrBuilder_PushU64(Allocator allocator, StrBuilder *builder, u64 num);
internal void       StrBuilder_PushF64(Allocator allocator, StrBuilder *builder, f64 num);

#endif