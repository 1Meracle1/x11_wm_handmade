#ifndef ENCODING_INI_H
#define ENCODING_INI_H

#include "../../core_defines.h"
#include "../../containers/array.h"
#include "../../containers/hash_map.h"

typedef struct
{
  enum
  {
    IniValue_i64,
    IniValue_u64,
    IniValue_f64,
    IniValue_bool,
    IniValue_String,
  } tag;
  union
  {
    i64    value_i64;
    u64    value_u64;
    f64    value_f64;
    bool   value_bool;
    String value_string;
  } data;
} IniValue;

HashMapTemplatePrefix(String, IniValue, IniValueMap, IniValueMap_);

typedef struct
{
  enum
  {
    IniSection_Map,
    IniSection_Array,
  } tag;
  union
  {
    IniValueMap  map;
    Array_String array;
  } data;
} IniSection;

HashMapTemplatePrefix(String, IniSection, IniMap, IniMap_);

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