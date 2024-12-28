#ifndef WM_CONFIG_H
#define WM_CONFIG_H

#include "../core/core.h"

typedef struct
{
  u64  minimum_width_tiling_window;
  f64  default_width_percent_available_width;
  u64  border_width;
  Vec3 border_default_color;
  Vec3 border_active_color;
  u64  inner_gap;
  u64  outer_gap_horizontal;
  u64  outer_gap_vertical;
} StyleConfig;

typedef struct
{
  StyleConfig style;
  ArrayString startup_actions;
  ArrayString keymap;
} Config;

/*
Returns true if the config parameter passed to it was updated
*/
internal bool LoadConfig(Allocator allocator, Config *config);

internal void PrintConfig(Allocator allocator, const Config* config);

#endif