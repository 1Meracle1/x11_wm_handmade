#include "hash_map.h"

internal u64 HashFromString(String key, u64 max)
{
  u64 hash = 5381;
  for (u64 i = 0; i < key.size; i += 1)
  {
    hash = ((hash << 5) + hash) + key.data[i];
  }
  hash = hash % max;
  return hash;
}