#include "ini.h"
#include "../../os/os.h"
#include <ctype.h>

internal IniValue IniValueFromString(String s)
{
  IniValue res = {0};
  if (s.size == 0)
  {
    res.tag = IniValue_String;
  }
  else
  {
    if (StrEquals(s, StrLit("true")))
    {
      res.tag             = IniValue_bool;
      res.data.value_bool = true;
    }
    else if (StrEquals(s, StrLit("false")))
    {
      res.tag             = IniValue_bool;
      res.data.value_bool = false;
    }
    else
    {
      bool contains_non_digit_chars = false;
      bool floating_num             = false;
      bool is_negative_num          = false;
      for (u64 i = 0; i < s.size; i += 1)
      {
        u8 c = s.data[i];
        if (!isdigit(c))
        {
          if (i == 0 && c == '-')
          {
            is_negative_num = -1;
          }
          else if (!floating_num && (c == '.' || c == ','))
          {
            floating_num = true;
          }
          else
          {
            contains_non_digit_chars = true;
            break;
          }
        }
      }

      if (contains_non_digit_chars)
      {
        res.tag               = IniValue_String;
        res.data.value_String = s;
      }
      else
      {
        if (floating_num)
        {
          f64 num = 0;
          F64FromStr(s, &num);
          res.tag            = IniValue_f64;
          res.data.value_f64 = num;
        }
        else
        {
          u64 num = 0;
          U64FromStr(s, &num);
          if (is_negative_num)
          {
            res.tag            = IniValue_i64;
            res.data.value_i64 = (i64)num * -1;
          }
          else
          {
            res.tag            = IniValue_u64;
            res.data.value_u64 = num;
          }
        }
      }
    }
  }
  return res;
}

internal String Unquote(String s)
{
  return StrTrim(s, StrLit("\""));
}

internal IniMap Ini_LoadMapFromPath(Allocator allocator, String path)
{
  String src = Fs_ReadFileFull(allocator, path);
  return Ini_LoadMapFromString(allocator, src);
}

internal IniMap Ini_LoadMapFromString(Allocator allocator, String src)
{
  IniMap      res          = IniMap_Init(allocator, 50);
  String      section_name = {0};
  IniSection  section      = {0};
  ArrayString lines        = StrSplit(allocator, src, StrLit("\n"));
  for (u64 i = 0; i < lines.size; i += 1)
  {
    String line = StrTrimSpaces(lines.data[i]);

    i64 comment_start_idx = StrIndexByte(line, ';');
    if (comment_start_idx != -1)
    {
      line = StrSubstrTill(line, comment_start_idx);
      if (line.size == 0)
      {
        continue;
      }
    }
    if (line.size == 0)
    {
      continue;
    }

    if (line.data[0] == '[')
    {
      i64 end_idx = StrIndexByte(line, ']');
      if (end_idx != -1)
      {
        if (section_name.size != 0)
        {
          IniMap_Push(allocator, &res, section_name, section);
        }
        if (line.size > 2 && line.data[1] == '[')
        {
          section_name       = StrSubstr(line, 2, end_idx);
          section.tag        = IniSection_Array;
          section.data.array = ArrayString_Init(allocator, 20);
        }
        else
        {
          section_name     = StrSubstr(line, 1, end_idx);
          section.tag      = IniSection_Map;
          section.data.map = IniValueMap_Init(allocator, 50);
        }
        continue;
      }
    }

    if (section.tag == IniSection_Array)
    {
      ArrayString_Push(allocator, &section.data.array, line);
    }
    else
    {
      i64 equal_sign = StrIndexByte(line, '=');
      if (equal_sign != -1)
      {
        String key       = Unquote(StrTrimSpaces(StrSubstrTill(line, equal_sign)));
        String value_str = Unquote(StrTrimSpaces(StrSubstrFrom(line, equal_sign + 1)));
        if (key.size != 0)
        {
          IniValue value = IniValueFromString(value_str);
          IniValueMap_Push(allocator, &section.data.map, key, value);
        }
      }
    }
  }
  if (section_name.size != 0)
  {
    IniMap_Push(allocator, &res, section_name, section);
  }
  ArrayString_Deinit(allocator, &lines);

  return res;
}

internal void Ini_DeinitMap(Allocator allocator, IniMap *map)
{
  IniMap_Deinit(allocator, map);
}
