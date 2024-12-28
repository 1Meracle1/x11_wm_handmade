#ifndef ARRAY_H
#define ARRAY_H

#include "../core_defines.h"

#define ArrayTemplate(type) ArrayTemplatePrefix(type, Array##type, Array##type##_)

#define ArrayTemplatePrefix(type, struct_name, funcs_prefix)                                       \
  ArrayTemplatePrefixIndexType(type, struct_name, funcs_prefix, u64)

#define ArrayTemplatePrefixIndexType(type, struct_name, funcs_prefix, index_type)                  \
  typedef struct                                                                                   \
  {                                                                                                \
    type      *data;                                                                               \
    index_type size;                                                                               \
    index_type capacity;                                                                           \
  } struct_name;                                                                                   \
                                                                                                   \
  internal struct_name funcs_prefix##Init(Allocator allocator, index_type capacity)                \
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
      index_type new_capacity = Max(array->capacity * 2, 1);                                       \
      type      *data         = Alloc(type, new_capacity);                                         \
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
  internal void funcs_prefix##UnorderedRemove(struct_name *array, index_type index)                \
  {                                                                                                \
    if (index + 1 < array->size)                                                                   \
    {                                                                                              \
      SwapT(array->data[index], array->data[array->size - 1], type);                               \
    }                                                                                              \
    array->size -= 1;                                                                              \
  }                                                                                                \
                                                                                                   \
  internal void funcs_prefix##OrderedRemove(struct_name *array, index_type index)                  \
  {                                                                                                \
    for (index_type i = index; i < array->size - 1; i += 1)                                        \
    {                                                                                              \
      SwapT(array->data[i], array->data[i + 1], type);                                             \
    }                                                                                              \
    array->size -= 1;                                                                              \
  }                                                                                                \
                                                                                                   \
  internal AllocationError funcs_prefix##Append(Allocator allocator, struct_name *dest,            \
                                                struct_name source)                                \
  {                                                                                                \
    AllocationError res = AllocationError_None;                                                    \
    if (source.size + dest->size > dest->capacity)                                                 \
    {                                                                                              \
      index_type new_capacity = source.size + dest->size;                                          \
      type      *data         = Alloc(type, new_capacity);                                         \
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
      for (index_type i = 0; i < source.size; i += 1)                                              \
      {                                                                                            \
        dest->data[dest->size] = source.data[i];                                                   \
        dest->size += 1;                                                                           \
      }                                                                                            \
    }                                                                                              \
    return res;                                                                                    \
  }                                                                                                \
                                                                                                   \
  internal bool funcs_prefix##IsEmpty(struct_name array)                                           \
  {                                                                                                \
    return array.size == 0;                                                                        \
  }                                                                                                \
                                                                                                   \
  internal type funcs_prefix##Nth(struct_name array, index_type index)                             \
  {                                                                                                \
    Assert(index < array.size);                                                                    \
    return array.data[index];                                                                      \
  }                                                                                                \
                                                                                                   \
  internal type *funcs_prefix##NthPtr(struct_name *array, index_type index)                        \
  {                                                                                                \
    Assert(index < array->size);                                                                   \
    return &array->data[index];                                                                    \
  }

#endif