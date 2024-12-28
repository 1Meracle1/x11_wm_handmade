#ifndef WM_WINDOW_H
#define WM_WINDOW_H

#include "../core/core.h"
#include <xcb/xproto.h>

typedef enum : u8
{
  WindowType_Normal   = 0,
  WindowType_Floating = 1,
  WindowType_Docked   = 2,
} WindowType;

typedef struct
{
  xcb_window_t *ids;
  i16          *xs;
  i16          *ys;
  u16          *widths;
  u16          *heights;
  WindowType   *window_types;
  u16           size;
  u16           capacity;
} WindowsSystem;

internal WindowsSystem   WindowsSystemInit(Allocator allocator, u16 capacity);
internal void            WindowsSystemDeinit(Allocator allocator, WindowsSystem *array);
internal AllocationError WindowsSystemPush(Allocator allocator, WindowsSystem *array,
                                           xcb_window_t id, i16 x, i16 y, u16 width, u16 height);
internal void            WindowsSystemUnorderedRemove(WindowsSystem *array, u16 index);

#endif