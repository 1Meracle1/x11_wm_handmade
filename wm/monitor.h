#ifndef WM_MONITOR_H
#define WM_MONITOR_H

#include "workspace.h"

ArrayTemplate(Workspace);

typedef struct
{
  xcb_atom_t     name;
  bool           primary;
  u16            output;
  i16            x;
  i16            y;
  u16            width;
  u16            height;
  ArrayWorkspace workspaces;
  u64            active_workspace;
} Monitor;

ArrayTemplate(Monitor);

internal void AddMonitor(Allocator allocator, String name, bool primary, u16 output, i16 x, i16 y,
                         u16 width, u16 height);

#endif