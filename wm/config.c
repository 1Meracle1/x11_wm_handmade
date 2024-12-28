#include "config.h"

u64 g_last_mod_time = 0;

internal bool LoadConfig(Allocator allocator, Config *config)
{
  bool   updated       = false;
  String root          = StrLit(PROJECT_DIR);
  String path          = Fs_PathJoin(allocator, root, StrLit("config.ini"));
  u64    last_mod_time = Fs_LastModifiedTime(allocator, path);
  if (g_last_mod_time < last_mod_time)
  {
    Debug("Loading updated config from disk");
    g_last_mod_time   = last_mod_time;
    IniMap config_map = Ini_LoadMapFromPath(allocator, path);
    // ArrayPair_StringToIniSection sections   = IniMap_KeyValuePairs(allocator, config_map);
    // Debugf("config sections: %zu", sections.size);
    // for (u64 i = 0; i < sections.size; i += 1)
    // {
    //   Debugf("%zuth: %.*s", i, StrFmtVal(sections.data[i].key));
    // }
    bool        valid         = true;
    IniSection *style_section = IniMap_Find(&config_map, StrLit("style"));
    if (style_section == NULL || style_section->tag != IniSection_Map)
    {
      valid = false;
      Errorf("Config: style section was not found in the config file at %.*s", StrFmtVal(path));
    }
    IniSection *startup_actions_section = IniMap_Find(&config_map, StrLit("startup_actions"));
    if (startup_actions_section == NULL || startup_actions_section->tag != IniSection_Array)
    {
      valid = false;
      Errorf("Config: startup_actions section was not found in the config file at %.*s",
             StrFmtVal(path));
    }
    IniSection *keymap_section = IniMap_Find(&config_map, StrLit("keymap"));
    if (keymap_section == NULL || keymap_section->tag != IniSection_Array)
    {
      valid = false;
      Errorf("Config: keymap section was not found in the config file at %.*s", StrFmtVal(path));
    }
    if (valid)
    {

#define PopulateField(field_name, type, section)                                                   \
  do                                                                                               \
  {                                                                                                \
    if (!valid)                                                                                    \
    {                                                                                              \
      continue;                                                                                    \
    }                                                                                              \
    IniValueMap map       = section->data.map;                                                     \
    IniValue   *ini_value = IniValueMap_Find(&map, StrLit(#field_name));                           \
    if (ini_value != NULL && ini_value->tag == IniValue_##type)                                    \
    {                                                                                              \
      config->style.field_name = ini_value->data.value_##type;                                     \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
      Error("Config: Failed to populate " #field_name " - incorrect or not present data");         \
      valid = false;                                                                               \
    }                                                                                              \
  } while (0)

      PopulateField(minimum_width_tiling_window, u64, style_section);
      PopulateField(default_width_percent_available_width, f64, style_section);
      PopulateField(border_width, u64, style_section);
      PopulateField(inner_gap, u64, style_section);
      PopulateField(outer_gap_horizontal, u64, style_section);
      PopulateField(outer_gap_vertical, u64, style_section);

      if (valid)
      {
        IniValueMap map       = style_section->data.map;
        IniValue   *ini_value = IniValueMap_Find(&map, StrLit("border_default_color"));
        if (ini_value != NULL && ini_value->tag == IniValue_String)
        {
          u8     dest[3];
          String hex_str = ini_value->data.value_String;
          if (hex_str.size < 2 || hex_str.data[0] != '#')
          {
            valid = false;
          }
          else
          {
            String hex = StrSubstrFrom(hex_str, 1);
            valid      = HexDecode(hex.data, hex.size, dest, sizeof dest);
          }
          if (valid)
          {
            config->style.border_default_color = (Vec3){dest[0], dest[1], dest[2]};
          }
          else
          {
            Error("Config: Failed to populate border_default_color - invalid hex data");
          }
        }
        else
        {
          Error("Config: Failed to populate border_default_color - incorrect or not present data");
          valid = false;
        }
      }

      if (valid)
      {
        IniValueMap map       = style_section->data.map;
        IniValue   *ini_value = IniValueMap_Find(&map, StrLit("border_active_color"));
        if (ini_value != NULL && ini_value->tag == IniValue_String)
        {
          u8     dest[3];
          String hex_str = ini_value->data.value_String;
          if (hex_str.size < 2 || hex_str.data[0] != '#')
          {
            valid = false;
          }
          else
          {
            String hex = StrSubstrFrom(hex_str, 1);
            valid      = HexDecode(hex.data, hex.size, dest, sizeof dest);
          }
          if (valid)
          {
            config->style.border_active_color = (Vec3){dest[0], dest[1], dest[2]};
          }
          else
          {
            Error("Config: Failed to populate border_active_color - invalid hex data");
          }
        }
        else
        {
          Error("Config: Failed to populate border_active_color - incorrect or not present data");
          valid = false;
        }
      }

      if (valid)
      {
        config->startup_actions = startup_actions_section->data.array;
        config->keymap          = keymap_section->data.array;
      }

#undef PopulateField

      updated = true;
    }
    IniMap_Deinit(allocator,&config_map);
  }
  return updated;
}

internal void PrintConfig(Allocator allocator, const Config* config)
{
  StrBuilder builder = StrBuilder_Init(allocator, 50);
  #define PushStr(str) StrBuilder_PushStr(allocator, &builder, str)
  #define PushLit(str) StrBuilder_PushStr(allocator, &builder, StrLit(str))
  #define PushU64(v) StrBuilder_PushU64(allocator, &builder, v)
  #define PushF64(v) StrBuilder_PushF64(allocator, &builder, v)
  {
    PushLit("style\n");
    PushLit("{\n");
    PushLit("\tminimum_width_tiling_window: ");
    PushU64(config->style.minimum_width_tiling_window);
    PushLit("\n");
    PushLit("\tdefault_width_percent_available_width: ");
    PushF64(config->style.default_width_percent_available_width);
    PushLit("\n");
    PushLit("\tborder_width: ");
    PushU64(config->style.border_width);
    PushLit("\n");
    PushLit("\tborder_default_color: [");
    PushU64(config->style.border_default_color.x);
    PushLit(", ");
    PushU64(config->style.border_default_color.y);
    PushLit(", ");
    PushU64(config->style.border_default_color.z);
    PushLit("]\n");
    PushLit("\tborder_active_color: [");
    PushU64(config->style.border_active_color.x);
    PushLit(", ");
    PushU64(config->style.border_active_color.y);
    PushLit(", ");
    PushU64(config->style.border_active_color.z);
    PushLit("]\n");
    PushLit("\tinner_gap: ");
    PushU64(config->style.inner_gap);
    PushLit("\n");
    PushLit("\touter_gap_horizontal: ");
    PushU64(config->style.outer_gap_horizontal);
    PushLit("\n");
    PushLit("\touter_gap_vertical: ");
    PushU64(config->style.outer_gap_vertical);
    PushLit("\n}\n");

    PushLit("startup_actions\n");
    PushLit("[\n");
    for(u64 i = 0; i < config->startup_actions.size; i += 1)
    {
      PushLit("\t");
      PushStr(config->startup_actions.data[i]);
      PushLit(",\n");
    }
    PushLit("]\n");

    PushLit("keymap\n");
    PushLit("[\n");
    for(u64 i = 0; i < config->keymap.size; i += 1)
    {
      PushLit("\t");
      PushStr(config->keymap.data[i]);
      PushLit(",\n");
    }
    PushLit("]\n");

    String s = StrBuilder_ToString(allocator, builder);
    Infof("Config:\n%.*s", StrFmtVal(s));
  }
  #undef PushF64
  #undef PushU64
  #undef PushLit
  #undef PushStr
  StrBuilder_Deinit(allocator,&builder);
}