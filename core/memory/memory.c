#include "memory.h"
#include "../os/os.h"
#include "../log/log.h"

internal b8 is_power_of_two(u64 x)
{
  return (x & (x - 1)) == 0;
}

internal u64 align_forward(u64 pos, u64 align)
{
  Assert(is_power_of_two(align));
  u64 aligned_pos = pos;
  u64 modulo      = aligned_pos & (align - 1);
  if (modulo != 0)
  {
    aligned_pos += align - modulo;
  }
  return aligned_pos;
}

internal Arena *ArenaInit(u64 size)
{
  u64   aligned_size        = align_forward(size, Megabytes(64));
  void *block               = OS_Reserve(aligned_size, NULL);
  u64   commit_granularity  = OS_PageSize();
  u64   initial_commit_size = commit_granularity;
  Assert(initial_commit_size >= sizeof(Arena));
  OS_Commit(block, initial_commit_size);
  Arena *arena              = (Arena *)block;
  arena->pos                = sizeof(Arena);
  arena->commit_pos         = initial_commit_size;
  arena->align              = 16;
  arena->size               = size;
  arena->commit_granularity = commit_granularity;
  return arena;
}

internal void ArenaDeinit(Arena *arena)
{
  OS_Release(arena, arena->size);
}

internal void *ArenaAlloc(Arena *arena, u64 size)
{
  void *result = NULL;
  if (size != 0)
  {
    u64 aligned_pos = align_forward(arena->pos, arena->align);
    if (aligned_pos + size <= arena->size)
    {
      u8 *base   = (u8 *)arena;
      result     = base + aligned_pos;
      arena->pos = aligned_pos + size;
      if (arena->pos > arena->commit_pos)
      {
        u64 size_to_commit =
            align_forward(arena->pos - arena->commit_pos, arena->commit_granularity);
        OS_Commit(base + arena->commit_pos, size_to_commit);
        arena->commit_pos += size_to_commit;
      }
      memset(result, 0, size);
    }
  }
  return result;
}

internal void ArenaPopTo(Arena *arena, u64 pos)
{
  u64 min_pos                      = sizeof(Arena);
  u64 new_pos                      = Max(min_pos, pos);
  arena->pos                       = new_pos;
  u64 pos_aligned_to_commit_chunks = align_forward(arena->pos, arena->commit_granularity);
  if (pos_aligned_to_commit_chunks + arena->commit_granularity <= arena->commit_pos)
  {
    u64 size_to_decommit = arena->commit_pos - pos_aligned_to_commit_chunks;
    OS_Decommit((u8 *)arena + pos_aligned_to_commit_chunks, size_to_decommit);
    arena->commit_pos -= size_to_decommit;
  }
}

internal void ArenaPop(Arena *arena, u64 size)
{
  u64 min_pos     = sizeof(Arena);
  u64 size_to_pop = Min(size, arena->pos);
  u64 new_pos     = arena->pos - size_to_pop;
  new_pos         = Max(new_pos, min_pos);
  ArenaPopTo(arena, new_pos);
}

internal void *_alloc(void *allocator, u64 size)
{
  Arena *arena = (Arena *)allocator;
  return ArenaAlloc(arena, size);
}

internal void _free(void *allocator, void *ptr, u64 size)
{
  (void)allocator;
  (void)ptr;
  (void)size;
}

internal Allocator ArenaAllocator(Arena *arena)
{
  Allocator allocator = {0};
  allocator.alloc     = _alloc;
  allocator.free      = _free;
  allocator.data      = (void *)arena;
  return allocator;
}

internal Temp TempBegin(Arena *arena)
{
  Temp temp  = {0};
  temp.arena = arena;
  temp.pos   = arena->pos;
  return temp;
}

internal void TempEnd(Temp temp)
{
  ArenaPopTo(temp.arena, temp.pos);
}