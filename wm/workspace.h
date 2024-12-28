#ifndef WM_WORKSPACE_H
#define WM_WORKSPACE_H

#include "../core/core.h"
#include <xcb/xproto.h>

ArrayTemplatePrefix(xcb_window_t, ArrayWindowId, ArrayWindowId_);

typedef struct
{
  i16 x;
  i16 y;
  u16 width;
  u16 height;
} Rect;

typedef struct
{
  u16           id;
  Rect          available_space;
  Rect          monitor_available_space;
  ArrayWindowId normal_mapped_windows;
  ArrayWindowId normal_unmapped_windows;
  ArrayWindowId floating_mapped_windows;
  ArrayWindowId floating_unmapped_windows;
  ArrayWindowId docked_mapped_windows;
  ArrayWindowId docked_unmapped_windows;
} Workspace;

internal Workspace WorkspaceInit(Allocator allocator, u16 id, Rect monitor_available_space);
internal void      WorkspaceDeinit(Allocator allocator, Workspace *workspace);
internal void WorkspaceAddFloatingWindow(Allocator allocator, Workspace *workspace, xcb_window_t id,
                                         Rect hint_rect);

#endif