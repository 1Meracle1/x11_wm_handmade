#ifndef ENCODING_INI_H
#define ENCODING_INI_H

#include "../../core_defines.h"
#include "../../containers/array.h"
#include "../../containers/hash_map.h"

typedef struct
{
  enum
  {
    IniValue_i64    = 0,
    IniValue_u64    = 1,
    IniValue_f64    = 2,
    IniValue_bool   = 3,
    IniValue_String = 4,
  } tag;
  union
  {
    i64    value_i64;
    u64    value_u64;
    f64    value_f64;
    bool   value_bool;
    String value_String;
  } data;
} IniValue;

EmptyKeyValueFuncTemplate(String, IniValue);
HashMapTemplateFull(String, IniValue, IniValueMap, IniValueMap_, HashFromString, StrEquals,
                    StrIsEmpty, EmptyKeyValueDefault_String_IniValue, u64);

typedef struct
{
  enum
  {
    IniSection_Map   = 0,
    IniSection_Array = 1,
  } tag;
  union
  {
    IniValueMap map;
    ArrayString array;
  } data;
} IniSection;

EmptyKeyValueFuncTemplate(String, IniSection);
HashMapTemplateFull(String, IniSection, IniMap, IniMap_, HashFromString, StrEquals,
                    StrIsEmpty, EmptyKeyValueDefault_String_IniSection, u64);

/*
Example:
  IniMap                        config_map = Ini_LoadMapFromPath(allocator, path);
  Array_Pair_StringToIniSection sections   = IniMap_KeyValuePairs(allocator, config_map);
  Debugf("config sections: %zu", sections.size);
  for (u64 i = 0; i < sections.size; i += 1)
  {
    Debugf("%zuth: %.*s", i, StrFmtVal(sections.data[i].key));
  }
  IniSection *keymap_section = IniMap_Find(&config_map, StrLit("keymap"));
  Assert(keymap_section != NULL);
*/
internal IniMap Ini_LoadMapFromPath(Allocator allocator, String path);
internal IniMap Ini_LoadMapFromString(Allocator allocator, String src);
internal void   Ini_DeinitMap(Allocator allocator, IniMap *map);

#endif