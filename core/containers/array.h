#ifndef ARRAY_H
#define ARRAY_H

#include "../core_defines.h"

#define ArrayTemplate(type) ArrayTemplatePrefix(type, Array##type, Array##type##_)

#define ArrayTemplatePrefix(type, struct_name, funcs_prefix)                                       \
  typedef struct                                                                                   \
  {                                                                                                \
    type *data;                                                                                    \
    u64   size;                                                                                    \
    u64   capacity;                                                                                \
  } struct_name;                                                                                   \
                                                                                                   \
  internal struct_name funcs_prefix##Init(Allocator allocator, u64 capacity)                       \
  {                                                                                                \
    Assert(capacity > 0);                                                                          \
    struct_name res;                                                                               \
    res.size     = 0;                                                                              \
    res.capacity = capacity;                                                                       \
    res.data     = Alloc(type, res.capacity);                                                      \
    if (!res.data)                                                                                 \
    {                                                                                              \
      res.capacity = 0;                                                                            \
    }                                                                                              \
    return res;                                                                                    \
  }                                                                                                \
                                                                                                   \
  internal struct_name funcs_prefix##InitDefault(Allocator allocator)                              \
  {                                                                                                \
    return funcs_prefix##Init(allocator, 1);                                                       \
  }                                                                                                \
                                                                                                   \
  internal void funcs_prefix##Deinit(Allocator allocator, struct_name *array)                      \
  {                                                                                                \
    if (array && array->capacity > 0)                                                              \
    {                                                                                              \
      Free(array->data, array->capacity);                                                          \
      array->capacity = 0;                                                                         \
      array->size     = 0;                                                                         \
    }                                                                                              \
  }                                                                                                \
                                                                                                   \
  internal void funcs_prefix##Reset(struct_name *array)                                            \
  {                                                                                                \
    if (array->size)                                                                               \
    {                                                                                              \
      memset(array->data, 0, sizeof(type) * array->size);                                          \
    }                                                                                              \
    array->size = 0;                                                                               \
  }                                                                                                \
                                                                                                   \
  internal AllocationError funcs_prefix##Push(Allocator allocator, struct_name *array, type str)   \
  {                                                                                                \
    Assert(array);                                                                                 \
    AllocationError res = AllocationError_None;                                                    \
    if (array->capacity == array->size)                                                            \
    {                                                                                              \
      u64   new_capacity = Max(array->capacity * 2, 1);                                            \
      type *data         = Alloc(type, new_capacity);                                              \
      if (!data)                                                                                   \
      {                                                                                            \
        res = AllocationError_OutOfMemory;                                                         \
      }                                                                                            \
      else                                                                                         \
      {                                                                                            \
        memcpy(data, array->data, sizeof(type) * array->size);                                     \
        Free(array->data, array->size);                                                            \
        array->data     = data;                                                                    \
        array->capacity = new_capacity;                                                            \
      }                                                                                            \
    }                                                                                              \
    if (res == AllocationError_None)                                                               \
    {                                                                                              \
      array->data[array->size] = str;                                                              \
      array->size += 1;                                                                            \
    }                                                                                              \
    return res;                                                                                    \
  }                                                                                                \
                                                                                                   \
  internal AllocationError funcs_prefix##Append(Allocator allocator, struct_name *dest,            \
                                                struct_name source)                                \
  {                                                                                                \
    AllocationError res = AllocationError_None;                                                    \
    if (source.size + dest->size > dest->capacity)                                                 \
    {                                                                                              \
      u64   new_capacity = source.size + dest->size;                                               \
      type *data         = Alloc(type, new_capacity);                                              \
      if (!data)                                                                                   \
      {                                                                                            \
        res = AllocationError_OutOfMemory;                                                         \
      }                                                                                            \
      else                                                                                         \
      {                                                                                            \
        memcpy(data, dest->data, sizeof(type) * dest->size);                                       \
        Free(dest->data, dest->size);                                                              \
        dest->data     = data;                                                                     \
        dest->capacity = new_capacity;                                                             \
      }                                                                                            \
    }                                                                                              \
    if (res == AllocationError_None)                                                               \
    {                                                                                              \
      for (u64 i = 0; i < source.size; i += 1)                                                     \
      {                                                                                            \
        dest->data[dest->size] = source.data[i];                                                   \
        dest->size += 1;                                                                           \
      }                                                                                            \
    }                                                                                              \
    return res;                                                                                    \
  }                                                                                                \
                                                                                                   \
  internal bool funcs_prefix##Empty(struct_name array)                                             \
  {                                                                                                \
    return array.size == 0;                                                                        \
  }                                                                                                \
                                                                                                   \
  internal type funcs_prefix##Nth(struct_name array, u64 index)                                    \
  {                                                                                                \
    Assert(index < array.size);                                                                    \
    return array.data[index];                                                                      \
  }                                                                                                \
                                                                                                   \
  internal type *funcs_prefix##NthPtr(struct_name *array, u64 index)                               \
  {                                                                                                \
    Assert(index < array->size);                                                                   \
    return &array->data[index];                                                                    \
  }

#endif