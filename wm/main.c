#include "../core/core.h"

#include "../core/core.c"

HashMapTemplate(String, i64);

#ifndef PROJECT_DIR
#define PROJECT_DIR ""
#endif

int main(void)
{
  Arena    *arena     = ArenaInit(Megabytes(1));
  Allocator allocator = ArenaAllocator(arena);

  String root = StrLit(PROJECT_DIR);
  Debugf("project directory: %s", root);
  String path            = Fs_PathJoin(allocator, root, StrLit("config.ini"));
  u64    last_mod_time   = Fs_LastModifiedTime(allocator, path);
  String last_mod_time_s = StringFromTimeRFC3339(allocator, last_mod_time);
  Debugf("last modified time: %s", last_mod_time_s);

  IniMap                        config_map = Ini_LoadMapFromPath(allocator, path);
  Array_Pair_StringToIniSection sections   = IniMap_KeyValuePairs(allocator, config_map);
  Debugf("config sections: %zu", sections.size);
  for (u64 i = 0; i < sections.size; i += 1)
  {
    Debugf("%zuth: %.*s", i, StrFmtVal(sections.data[i].key));
  }
  IniSection *keymap_section = IniMap_Find(&config_map, StrLit("keymap"));
  Assert(keymap_section != NULL);

  // String       core_dir = Fs_PathJoin(allocator, root, StrLit("core"));
  // Array_String elements = Array_String_Init(allocator, 10);
  // FsErrors     err      = Fs_ReadDir(allocator, core_dir, &elements);
  // if (err != FsErrors_None)
  // {
  //   Errorf("Failed to read dir elements, err: %d, path: %s", err, core_dir);
  // }
  // Debugf("Retrieved %zu items from folder %s", elements.size, core_dir);
  // for (u64 i = 0; i < elements.size; i += 1)
  // {
  //   String elem_path = elements.data[i];
  //   bool   is_dir    = Fs_IsDir(allocator, elem_path);
  //   Debugf("%zuth element, is_dir: %d, path: %s", i, is_dir, elem_path);
  // }

  // String wm_main_file = Fs_PathJoin(allocator, root, StrLit("wm/main.c"));
  // String wm_main_cont = Fs_ReadFileFull(allocator, wm_main_file);
  // Debugf("wm's main.c file:\n%s", wm_main_cont);
  // Array_String lines = StrSplit(allocator, wm_main_cont, StrLit("\n"));
  // Debugf("lines: %zu", lines.size);
  // for (u64 i = 0; i < lines.size; i += 1)
  // {
  //   Debugf("line: %zu, str: %.*s", i, StrFmtVal(lines.data[i]));
  // }

  // const u64 frame_time = Seconds(1) / 60;
  // for (;;)
  // {
  //   u64 frame_start = TimeNow();

  //   Temp temp = TempBegin(arena);
  //   {
  //     int *heap_allocated_int = Alloc(int, 1);
  //     int *array_of_ints      = Alloc(int, 10);
  //   }
  //   {
  //     String s     = StrLit("123");
  //     u64    value = 0;
  //     Assert(U64FromStr(s, &value) == StrParseError_None);
  //     Assert(value == 123);
  //   }
  //   {
  //     String s     = StrLit("123.2");
  //     f64    value = 0;
  //     Assert(F64FromStr(s, &value) == StrParseError_None);
  //     Debugf("%f", value);
  //   }
  //   {
  //     String s     = StrLit("");
  //     f64    value = 0;
  //     Assert(F64FromStr(s, &value) == StrParseError_EmptyString);
  //   }
  //   {
  //     String s     = StrLit(":");
  //     f64    value = 0;
  //     Assert(F64FromStr(s, &value) == StrParseError_InvalidNumberString);
  //   }
  //   {
  //     HashMap_StringToi64 hash_map =
  //         HashMap_StringToi64_InitDefault(allocator, HashFromString, StrEquals, StrIsEmpty);
  //     String key            = StrLit("Hello");
  //     i64    value          = -2;
  //     i64   *inserted_value = HashMap_StringToi64_Push(allocator, &hash_map, key, value);
  //     Assert(value == *inserted_value);
  //   }
  //   {
  //     HashMap_StringToi64 hash_map =
  //         HashMap_StringToi64_Init(allocator, 1, HashFromString, StrEquals, StrIsEmpty);
  //     String key1            = StrLit("Hello");
  //     i64    value1          = -2;
  //     i64   *inserted_value1 = HashMap_StringToi64_Push(allocator, &hash_map, key1, value1);
  //     Assert(value1 == *inserted_value1);

  //     String key2            = StrLit("Someone");
  //     i64    value2          = 50;
  //     i64   *inserted_value2 = HashMap_StringToi64_Push(allocator, &hash_map, key2, value2);
  //     Assert(value2 == *inserted_value2);

  //     Assert(*HashMap_StringToi64_Find(&hash_map, key1) == value1);
  //     Assert(*HashMap_StringToi64_Find(&hash_map, key2) == value2);
  //     Assert(HashMap_StringToi64_Find(&hash_map, StrLit("some other key")) == NULL);
  //   }
  //   {
  //     u64 now = TimeNow();
  //     Debugf("time now nanos: %zu", now);
  //     String s = StringFromTime(allocator, now);
  //     Debugf("time now date: %.*s", StrFmtVal(s));
  //   }
  //   {
  //     u64 now = TimeNow();
  //     Debugf("time now nanos: %zu", now);
  //     String s = StringFromTimeRFC3339(allocator, now);
  //     Debugf("time now date RFC3339: %.*s", StrFmtVal(s));
  //   }

  //   TempEnd(temp);

  //   u64 frame_end = TimeNow();
  //   u64 diff      = frame_end - frame_start;
  //   if (diff < frame_time)
  //   {
  //     Sleep(frame_time - diff);
  //   }
  // }
  ArenaDeinit(arena);
  return 0;
}