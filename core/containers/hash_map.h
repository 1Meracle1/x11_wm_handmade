#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "../core_defines.h"
#include "string.h"

/*
Examples:
  {
    HashMap_StringToi64 hash_map =
        HashMap_StringToi64_InitDefault(allocator, HashFromString, StrEquals, StrIsEmpty);
    String key            = StrLit("Hello");
    i64    value          = -2;
    i64   *inserted_value = HashMap_StringToi64_Push(allocator, &hash_map, key, value);
    Assert(value == *inserted_value);
  }
  {
    HashMap_StringToi64 hash_map =
        HashMap_StringToi64_Init(allocator, 1, HashFromString, StrEquals, StrIsEmpty);
    String key1            = StrLit("Hello");
    i64    value1          = -2;
    i64   *inserted_value1 = HashMap_StringToi64_Push(allocator, &hash_map, key1, value1);
    Assert(value1 == *inserted_value1);

    String key2            = StrLit("Someone");
    i64    value2          = 50;
    i64   *inserted_value2 = HashMap_StringToi64_Push(allocator, &hash_map, key2, value2);
    Assert(value2 == *inserted_value2);

    Assert(*HashMap_StringToi64_Find(&hash_map, key1) == value1);
    Assert(*HashMap_StringToi64_Find(&hash_map, key2) == value2);
    Assert(HashMap_StringToi64_Find(&hash_map, StrLit("some other key")) == NULL);
  }
*/

internal u64 HashFromString(String key, u64 max);

typedef u64 (*HashKey)(String key, u64 max);
typedef bool (*KeyEqualTo)(String lhs, String rhs);
typedef bool (*KeyIsEmpty)(String key);

#define HashMapTemplate(type_key, type_value)                                                      \
  HashMapTemplatePrefix(type_key, type_value, HashMap_##type_key##To##type_value,                  \
                        HashMap_##type_key##To##type_value##_)

#define HashMapTemplatePrefix(type_key, type_value, struct_name, funcs_prefix)                      \
  typedef struct                                                                                    \
  {                                                                                                 \
    type_key   *keys;                                                                               \
    type_value *values;                                                                             \
    u64         capacity;                                                                           \
    HashKey     hash_func;                                                                          \
    KeyEqualTo  key_equals_func;                                                                    \
    KeyIsEmpty  key_is_empty_func;                                                                  \
  } struct_name;                                                                                    \
                                                                                                    \
  internal struct_name funcs_prefix##Init(Allocator allocator, u64 capacity, HashKey hash_func,     \
                                          KeyEqualTo key_equals_func,                               \
                                          KeyIsEmpty key_is_empty_func)                             \
  {                                                                                                 \
    Assert(capacity > 0);                                                                           \
    struct_name res;                                                                                \
    res.capacity          = capacity;                                                               \
    res.keys              = Alloc(type_key, res.capacity);                                          \
    res.values            = Alloc(type_value, res.capacity);                                        \
    res.hash_func         = hash_func;                                                              \
    res.key_equals_func   = key_equals_func;                                                        \
    res.key_is_empty_func = key_is_empty_func;                                                      \
    if (!res.keys || !res.values)                                                                   \
    {                                                                                               \
      res.capacity = 0;                                                                             \
    }                                                                                               \
    return res;                                                                                     \
  }                                                                                                 \
                                                                                                    \
  internal struct_name funcs_prefix##InitDefault(Allocator allocator, HashKey hash_func,            \
                                                 KeyEqualTo key_equals_func,                        \
                                                 KeyIsEmpty key_is_empty_func)                      \
  {                                                                                                 \
    return funcs_prefix##Init(allocator, 255, hash_func, key_equals_func, key_is_empty_func);       \
  }                                                                                                 \
                                                                                                    \
  internal void funcs_prefix##Deinit(Allocator allocator, struct_name *map)                         \
  {                                                                                                 \
    if (map && map->capacity > 0)                                                                   \
    {                                                                                               \
      Free(map->keys, map->capacity);                                                               \
      Free(map->values, map->capacity);                                                             \
      map->capacity = 0;                                                                            \
    }                                                                                               \
  }                                                                                                 \
                                                                                                    \
  internal void funcs_prefix##Reset(struct_name *map)                                               \
  {                                                                                                 \
    memset(map->keys, 0, sizeof(String) * map->capacity);                                           \
    memset(map->values, 0, sizeof(i64) * map->capacity);                                            \
  }                                                                                                 \
                                                                                                    \
  internal type_value *funcs_prefix##Find(struct_name *map, type_key key)                           \
  {                                                                                                 \
    type_value *res  = {0};                                                                         \
    u64         hash = map->hash_func(key, map->capacity);                                          \
    for (u64 i = hash; i < map->capacity; i += 1)                                                   \
    {                                                                                               \
      if (map->key_equals_func(map->keys[i], key))                                                  \
      {                                                                                             \
        res = &map->values[i];                                                                      \
        break;                                                                                      \
      }                                                                                             \
    }                                                                                               \
    return res;                                                                                     \
  }                                                                                                 \
                                                                                                    \
  internal type_value *funcs_prefix##Push(Allocator allocator, struct_name *map, type_key key,      \
                                          type_value value)                                         \
  {                                                                                                 \
    Assert(map);                                                                                    \
    type_value *res = funcs_prefix##Find(map, key);                                                 \
    if (res)                                                                                        \
    {                                                                                               \
      *res = value;                                                                                 \
    }                                                                                               \
    else                                                                                            \
    {                                                                                               \
      u64 hash = map->hash_func(key, map->capacity);                                                \
      if (map->key_is_empty_func(map->keys[hash]))                                                  \
      {                                                                                             \
        map->keys[hash]   = key;                                                                    \
        map->values[hash] = value;                                                                  \
        res               = &map->values[hash];                                                     \
      }                                                                                             \
      else                                                                                          \
      {                                                                                             \
        bool inserted = false;                                                                      \
        for (u64 i = hash + 1; i < map->capacity; i += 1)                                           \
        {                                                                                           \
          if (map->key_is_empty_func(map->keys[i]))                                                 \
          {                                                                                         \
            map->keys[i]   = key;                                                                   \
            map->values[i] = value;                                                                 \
            res            = &map->values[i];                                                       \
            inserted       = true;                                                                  \
            break;                                                                                  \
          }                                                                                         \
        }                                                                                           \
        if (!inserted)                                                                              \
        {                                                                                           \
          struct_name new_map = funcs_prefix##Init(allocator, map->capacity * 2, map->hash_func,    \
                                                   map->key_equals_func, map->key_is_empty_func);   \
          for (u64 i = 0; i < map->capacity; i += 1)                                                \
          {                                                                                         \
            if (!map->key_is_empty_func(map->keys[i]))                                              \
            {                                                                                       \
              funcs_prefix##Push(allocator, &new_map, map->keys[i], map->values[i]);                \
            }                                                                                       \
          }                                                                                         \
          funcs_prefix##Deinit(allocator, map);                                                     \
          *map = new_map;                                                                           \
                                                                                                    \
          res = funcs_prefix##Push(allocator, map, key, value);                                     \
        }                                                                                           \
      }                                                                                             \
    }                                                                                               \
    return res;                                                                                     \
  }                                                                                                 \
                                                                                                    \
  typedef struct                                                                                    \
  {                                                                                                 \
    type_key   key;                                                                                 \
    type_value value;                                                                               \
  } Pair_##type_key##To##type_value;                                                                \
  ArrayTemplate(Pair_##type_key##To##type_value);                                                   \
                                                                                                    \
  internal ArrayPair_##type_key##To##type_value funcs_prefix##KeyValuePairs(Allocator   allocator, \
                                                                             struct_name map)       \
  {                                                                                                 \
    ArrayPair_##type_key##To##type_value pairs =                                                    \
        ArrayPair_##type_key##To##type_value##_Init(allocator, map.capacity);                       \
    for (u64 i = 0; i < map.capacity; i += 1)                                                       \
    {                                                                                               \
      if (!map.key_is_empty_func(map.keys[i]))                                                      \
      {                                                                                             \
        Pair_##type_key##To##type_value p = {0};                                                    \
        p.key                             = map.keys[i];                                            \
        p.value                           = map.values[i];                                          \
        ArrayPair_##type_key##To##type_value##_Push(allocator, &pairs, p);                          \
      }                                                                                             \
    }                                                                                               \
    return pairs;                                                                                   \
  }

#endif