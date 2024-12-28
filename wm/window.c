#include "window.h"

internal WindowsSystem WindowsSystemInit(Allocator allocator, u16 capacity)
{
  Assert(capacity > 0);
  WindowsSystem res;
  res.size         = 0;
  res.capacity     = capacity;
  res.ids          = Alloc(xcb_window_t, res.capacity);
  res.xs           = Alloc(i16, res.capacity);
  res.ys           = Alloc(i16, res.capacity);
  res.widths       = Alloc(u16, res.capacity);
  res.heights      = Alloc(u16, res.capacity);
  res.window_types = Alloc(WindowType, res.capacity);
  if (!res.ids || !res.xs || !res.widths || !res.heights)
  {
    res.capacity = 0;
  }
  return res;
}

internal void WindowsSystemDeinit(Allocator allocator, WindowsSystem *array)
{
  if (array && array->capacity > 0)
  {
    Free(array->ids, array->capacity);
    Free(array->xs, array->capacity);
    Free(array->ys, array->capacity);
    Free(array->widths, array->capacity);
    Free(array->heights, array->capacity);
    Free(array->window_types, array->capacity);
    array->capacity = 0;
    array->size     = 0;
  }
}

internal AllocationError WindowsSystemPush(Allocator allocator, WindowsSystem *array,
                                           xcb_window_t id, i16 x, i16 y, u16 width, u16 height)
{
  Assert(array);
  AllocationError res = AllocationError_None;
  if (array->capacity == array->size)
  {
    u16 new_capacity = Max(array->capacity * 2, 1);
#define _Realloc(field_name, type)                                                                 \
  if (res == AllocationError_None)                                                                 \
  {                                                                                                \
    type *field_name = Alloc(type, new_capacity);                                                  \
    if (!field_name)                                                                               \
    {                                                                                              \
      res = AllocationError_OutOfMemory;                                                           \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
      memcpy(field_name, array->field_name, sizeof(array->field_name[0]) * array->size);           \
      Free(array->field_name, array->size);                                                        \
      array->field_name = field_name;                                                              \
      array->capacity   = new_capacity;                                                            \
    }                                                                                              \
  }

    _Realloc(ids, xcb_window_t);
    _Realloc(xs, i16);
    _Realloc(ys, i16);
    _Realloc(widths, u16);
    _Realloc(heights, u16);
    _Realloc(window_types, WindowType);

#undef _Realloc
  }
  if (res == AllocationError_None)
  {
    array->ids[array->size] = id;
    array->size += 1;
  }
  return res;
}

internal void WindowsSystemUnorderedRemove(WindowsSystem *array, u16 index)
{
  if (index + 1 < array->size)
  {
    SwapT(array->ids[index], array->ids[array->size - 1], xcb_window_t);
    SwapT(array->xs[index], array->xs[array->size - 1], i16);
    SwapT(array->ys[index], array->ys[array->size - 1], i16);
    SwapT(array->widths[index], array->widths[array->size - 1], u16);
    SwapT(array->heights[index], array->heights[array->size - 1], u16);
    SwapT(array->window_types[index], array->window_types[array->size - 1], u16);
  }
  array->size -= 1;
}