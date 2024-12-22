#ifndef MEMORY_H
#define MEMORY_H

#include "../core_defines.h"

typedef struct
{
  void *(*alloc)(void *allocator, u64 size);
  void (*free)(void *allocator, void *ptr, u64 size);
  void *data;
} Allocator;

/*
Example:
    int *heap_allocated_int = Alloc(int, 1);
    int *array_of_ints      = Alloc(int, 10);
*/
#define Alloc(type, count) (type *)allocator.alloc(allocator.data, sizeof(type) * (count))
#define Free(ptr, count) allocator.free(allocator.data, (ptr), sizeof(*ptr) * (count))

typedef struct
{
  u64 pos;
  u64 commit_pos;
  u64 align;
  u64 size;
  u64 commit_granularity;
} Arena;

internal Arena *ArenaInit(u64 size);
internal void   ArenaDeinit(Arena *arena);
internal void  *ArenaAlloc(Arena *arena, u64 size);
internal void   ArenaPopTo(Arena *arena, u64 pos);
internal void   ArenaPop(Arena *arena, u64 size);

internal Allocator ArenaAllocator(Arena *arena);

typedef struct
{
  Arena *arena;
  u64    pos;
} Temp;

internal Temp TempBegin(Arena *arena);
internal void TempEnd(Temp temp);

typedef enum
{
  AllocationError_None,
  AllocationError_OutOfMemory,
  AllocationError_InvalidArgument,
  AllocationError_InvalidPointer,
} AllocationError;

// void _example()
// {
//   Arena    *arena     = ArenaInit(Megabytes(1));
//   Allocator allocator = ArenaAllocator(arena);
//   Temp      temp      = TempBegin(arena);
//   {
//     int *heap_allocated_int = Alloc(int);

//     int **array_of_ints = Make(int, 10);
//   }
//   TempEnd(temp);
//   ArenaDeinit(arena);
// }

#endif