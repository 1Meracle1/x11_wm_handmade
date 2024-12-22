#ifndef ARRAY_H
#define ARRAY_H

#include "../core_defines.h"

#define ArrayTemplate(type)                                                                        \
  typedef struct                                                                                   \
  {                                                                                                \
    type *data;                                                                                    \
    u64   size;                                                                                    \
    u64   capacity;                                                                                \
  } Array_##type;                                                                                  \
                                                                                                   \
  internal Array_##type Array_##type##_Init(Allocator allocator, u64 capacity)                     \
  {                                                                                                \
    Assert(capacity > 0);                                                                          \
    Array_##type res;                                                                              \
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
  internal Array_##type Array_##type##_InitDefault(Allocator allocator)                            \
  {                                                                                                \
    return Array_##type##_Init(allocator, 1);                                                      \
  }                                                                                                \
                                                                                                   \
  internal void Array_##type##_Deinit(Allocator allocator, Array_##type *array)                    \
  {                                                                                                \
    if (array && array->capacity > 0)                                                              \
    {                                                                                              \
      Free(array->data, array->capacity);                                                              \
      array->capacity = 0;                                                                         \
      array->size     = 0;                                                                         \
    }                                                                                              \
  }                                                                                                \
                                                                                                   \
  internal void Array_##type##_Reset(Array_##type *array)                                          \
  {                                                                                                \
    if (array->size)                                                                               \
    {                                                                                              \
      memset(array->data, 0, sizeof(type) * array->size);                                          \
    }                                                                                              \
    array->size = 0;                                                                               \
  }                                                                                                \
                                                                                                   \
  internal AllocationError Array_##type##_Push(Allocator allocator, Array_##type *array, type str) \
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
  internal AllocationError Array_##type##_Append(Allocator allocator, Array_##type *dest,          \
                                                 Array_##type source)                              \
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
  internal bool Array_##type##_Empty(Array_##type array)                                           \
  {                                                                                                \
    return array.size == 0;                                                                        \
  }                                                                                                \
                                                                                                   \
  internal type Array_##type##_Nth(Array_##type array, u64 index)                                  \
  {                                                                                                \
    Assert(index < array.size);                                                                    \
    return array.data[index];                                                                      \
  }                                                                                                \
                                                                                                   \
  internal type *Array_##type##_NthPtr(Array_##type *array, u64 index)                             \
  {                                                                                                \
    Assert(index < array->size);                                                                   \
    return &array->data[index];                                                                    \
  }

#endif