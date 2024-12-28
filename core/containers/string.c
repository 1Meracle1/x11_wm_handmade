#include "string.h"
#include <ctype.h>
#include <math.h>

internal char *CstrFromStr(Allocator allocator, String s)
{
  char *res = {0};
  if (s.size > 0)
  {
    res = Alloc(char, s.size + 1);
    if (res)
    {
      memcpy(res, s.data, sizeof(char) * s.size);
      res[s.size] = '\0';
    }
  }
  return res;
}

internal String StrClone(Allocator allocator, String s)
{
  String res = {0};
  res.data   = Alloc(u8, s.size);
  if (res.data)
  {
    memcpy(res.data, s.data, sizeof(u8) * s.size);
    res.size = s.size;
  }
  return res;
}

internal String StrFromCstrClone(Allocator allocator, char *cstr, u64 size)
{
  String s = {0};
  if (size > 0)
  {
    s.data = Alloc(u8, size);
    s.size = size;
    memcpy(s.data, cstr, size);
  }
  return s;
}

internal bool StrEquals(String lhs, String rhs)
{
  bool res = true;
  if (lhs.size != rhs.size)
  {
    res = false;
  }
  if (lhs.size != 0 && res)
  {
    int cmp_res = memcmp(lhs.data, rhs.data, lhs.size);
    if (cmp_res != 0)
    {
      res = false;
    }
  }
  return res;
}

internal bool StrIsEmpty(String s)
{
  return s.size == 0;
}

internal String StrSubstr(String s, u64 start, u64 end)
{
  String res;
  u64    actual_end   = Min(s.size, end);
  u64    actual_start = Min(start, end);
  res.data            = s.data + actual_start;
  res.size            = actual_end - actual_start;
  return res;
}

internal String StrSubstrFrom(String s, u64 start)
{
  return StrSubstr(s, start, s.size);
}

internal String StrSubstrTill(String s, u64 end)
{
  return StrSubstr(s, 0, end);
}

internal u64 StrFindSubStr(String haystack, String needle, u64 min)
{
  u64 res = haystack.size;
  for (u64 i = min; i < haystack.size; i += 1)
  {
    String s = StrSubstr(haystack, i, i + needle.size);
    if (StrEquals(s, needle))
    {
      res = i;
      break;
    }
  }
  return res;
}

internal String StrTrimLeft(String s, String trimmed_chars)
{
  u64 left = 0;
  for (u64 i = 0; i < s.size; i += 1)
  {
    bool skipped = false;
    for (u64 j = 0; j < trimmed_chars.size; j += 1)
    {
      if (s.data[i] == trimmed_chars.data[j])
      {
        skipped = true;
        break;
      }
    }
    if (skipped != true)
    {
      left = i;
      break;
    }
  }
  String res = StrSubstr(s, left, s.size);
  return res;
}

internal String StrTrimRight(String s, String trimmed_chars)
{
  u64 right = s.size;
  for (i64 i = ((i64)s.size) - 1; i >= 0; i -= 1)
  {
    bool skipped = false;
    for (u64 j = 0; j < trimmed_chars.size; j += 1)
    {
      if (s.data[i] == trimmed_chars.data[j])
      {
        skipped = true;
        break;
      }
    }
    if (skipped != true)
    {
      right = (u64)i;
      break;
    }
  }
  String res = StrSubstr(s, 0, right + 1);
  return res;
}

internal String StrTrim(String s, String trimmed_chars)
{
  String left = StrTrimLeft(s, trimmed_chars);
  String res  = StrTrimRight(left, trimmed_chars);
  return res;
}

internal String StrTrimSpaces(String s)
{
  return StrTrim(s, StrLit(" \t\n\r"));
}

internal String StrJoin(Allocator allocator, String lhs, String sep, String rhs)
{
  String res = {0};
  res.size   = lhs.size + sep.size + rhs.size;
  res.data   = Alloc(u8, res.size);
  u64 pos    = 0;
  if (lhs.size != 0)
  {
    memcpy(res.data, lhs.data, lhs.size);
    pos += lhs.size;
  }
  if (sep.size != 0)
  {
    memcpy(res.data + pos, sep.data, sep.size);
    pos += sep.size;
  }
  if (rhs.size != 0)
  {
    memcpy(res.data + pos, rhs.data, rhs.size);
  }
  return res;
}

internal i64 StrIndexByte(String s, u8 byte)
{
  i64 res = -1;
  for (u64 i = 0; i < s.size; i += 1)
  {
    if (s.data[i] == byte)
    {
      res = (i64)i;
      break;
    }
  }
  return res;
}

internal u64 HashFromString(String key, u64 max)
{
  u64 hash = 5381;
  for (u64 i = 0; i < key.size; i += 1)
  {
    hash = ((hash << 5) + hash) + key.data[i];
  }
  hash = hash % max;
  return hash;
}

internal StrParseError U64FromStr(String s, u64 *value)
{
  StrParseError err = StrParseError_None;
  if (s.size == 0)
  {
    err = StrParseError_EmptyString;
  }

  if (err == StrParseError_None)
  {
    *value = 0;
    for (u64 i = 0; i < s.size; i += 1)
    {
      char c = s.data[i];
      if (!isdigit(c))
      {
        err = StrParseError_InvalidNumberString;
        break;
      }
      *value *= 10;
      *value += c - '0';
    }
  }

  return err;
}

internal StrParseError F64FromStr(String s, f64 *value)
{
  StrParseError err = StrParseError_None;
  if (s.size == 0)
  {
    err = StrParseError_EmptyString;
  }

  u64 split_idx = s.size;
  for (u64 i = 0; i < s.size; i += 1)
  {
    u8 c = s.data[i];
    if (c == '.' || c == ',')
    {
      if (split_idx == s.size)
      {
        split_idx = i;
      }
      else
      {
        err = StrParseError_InvalidNumberString;
        break;
      }
    }
  }

  if (err == StrParseError_None)
  {
    u64    floating_num = 0;
    String floating_str = StrSubstr(s, split_idx + 1, s.size);
    if (floating_str.size != 0)
    {
      err = U64FromStr(floating_str, &floating_num);
    }

    String whole_str = StrSubstr(s, 0, split_idx);
    u64    whole_num = 0;
    if (err == StrParseError_None)
    {
      err = U64FromStr(whole_str, &whole_num);
    }
    if (err == StrParseError_None)
    {
      *value = (f64)whole_num;
      if (floating_str.size != 0)
      {
        *value += (f64)floating_num / (pow(10, floating_str.size));
      }
    }
  }

  return err;
}

internal String StringFromU64(Allocator allocator, u64 value)
{
  String s;
  if (value == 0)
  {
    s = StrLit("0");
  }
  else
  {
    u64 size = 0;
    u64 num  = value;
    while (num != 0)
    {
      size += 1;
      num /= 10;
    }

    s.size = size;
    s.data = Alloc(u8, size);

    u64 pos = 0;
    num     = value;
    while (num != 0)
    {
      pos += 1;
      u64 digit          = num % 10;
      s.data[size - pos] = digit + '0';
      num /= 10;
    }
  }
  return s;
}

internal String StringFromF64(Allocator allocator, f64 value, u64 precision)
{
  String s = {0};

  u64  size        = 0;
  bool is_negative = false;
  if (value < 0)
  {
    is_negative = true;
    value       = -value;
    size += 1;
  }

  u64 integer_part    = (u64)value;
  f64 fractional_part = value - (f64)integer_part;

  String integer_part_str = StringFromU64(allocator, integer_part);
  size += integer_part_str.size + 1 + precision;
  s.data = Alloc(u8, size);
  s.size = size;

  u64 pos = 0;
  if (is_negative)
  {
    s.data[pos] = '-';
    pos += 1;
  }
  for (u64 i = 0; i < integer_part_str.size; i += 1)
  {
    s.data[pos] = integer_part_str.data[i];
    pos += 1;
  }
  s.data[pos] = '.';
  pos += 1;

  for (u64 i = 0; i < precision; i += 1)
  {
    fractional_part *= 10;
    u64 frac_digit = (u64)fractional_part;
    s.data[pos]    = frac_digit + '0';
    pos += 1;
    fractional_part -= frac_digit;
  }

  return s;
}

internal String ArrayString_Join(Allocator allocator, ArrayString arr, String sep)
{
  String res = {0};
  for (u64 i = 0; i < arr.size; i += 1)
  {
    res.size += arr.data[i].size;
    res.size += sep.size;
  }
  res.data = Alloc(u8, res.size);
  u64 pos  = 0;
  for (u64 i = 0; i < arr.size; i += 1)
  {
    memcpy(res.data + pos, arr.data[i].data, arr.data[i].size);
    pos += arr.data[i].size;
    if (sep.size != 0)
    {
      memcpy(res.data + pos, sep.data, sep.size);
      pos += sep.size;
    }
  }
  return res;
}

internal ArrayString StrSplit(Allocator allocator, String s, String sep)
{
  Assert(sep.size != 0);
  ArrayString res = ArrayString_Init(allocator, 1);
  for (u64 i = 0; i < s.size; i += sep.size)
  {
    u64 pos = StrFindSubStr(s, sep, i);
    if (pos != s.size)
    {
      ArrayString_Push(allocator, &res, StrSubstr(s, i, pos));
      i = pos;
    }
    else
    {
      break;
    }
  }
  return res;
}

internal ArrayString StrSplitInitCapacity(Allocator allocator, String s, String sep,
                                          u64 expected_elements)
{
  Assert(sep.size != 0);
  ArrayString res = ArrayString_Init(allocator, expected_elements);
  for (u64 i = 0; i < s.size; i += sep.size)
  {
    u64 pos = StrFindSubStr(s, sep, i);
    if (pos != s.size)
    {
      ArrayString_Push(allocator, &res, StrSubstr(s, i, pos));
      i = pos;
    }
    else
    {
      break;
    }
  }
  return res;
}

internal StrBuilder StrBuilder_Init(Allocator allocator, u64 min_capacity)
{
  StrBuilder builder = {0};
  builder.data       = ArrayString_Init(allocator, min_capacity);
  return builder;
}

internal void StrBuilder_Deinit(Allocator allocator, StrBuilder *builder)
{
  ArrayString_Deinit(allocator, &builder->data);
}

internal String StrBuilder_ToString(Allocator allocator, StrBuilder builder)
{
  return ArrayString_Join(allocator, builder.data, StrLit(""));
}

internal void StrBuilder_PushStr(Allocator allocator, StrBuilder *builder, String s)
{
  String cloned_s = StrClone(allocator, s);
  ArrayString_Push(allocator, &builder->data, cloned_s);
}

internal void StrBuilder_PushCstr(Allocator allocator, StrBuilder *builder, char *cstr)
{
  String s = StrFromCstrClone(allocator, cstr, strlen(cstr));
  ArrayString_Push(allocator, &builder->data, s);
}

internal void StrBuilder_PushU64(Allocator allocator, StrBuilder *builder, u64 num)
{
  String s = StringFromU64(allocator, num);
  ArrayString_Push(allocator, &builder->data, s);
}

internal void StrBuilder_PushF64(Allocator allocator, StrBuilder *builder, f64 num)
{
  String s = StringFromF64(allocator, num, 2);
  ArrayString_Push(allocator, &builder->data, s);
}