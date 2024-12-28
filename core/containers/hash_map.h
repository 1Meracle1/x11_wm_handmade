#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "../core_defines.h"
#include "string.h"

/*
Examples:
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

// typedef u64 (*HashKey)(String key, u64 max);
// typedef bool (*KeyEqualTo)(String lhs, String rhs);
// typedef bool (*KeyIsEmpty)(String key);
// typedef void (*EmptyKeyValue)(String* key, String* value);

#define EmptyKeyValueFuncTemplate(key_type, value_type)                                            \
  internal void EmptyKeyValueDefault_##key_type##_##value_type(key_type *key, value_type *value)   \
  {                                                                                                \
    memset((u8 *)key, 0, sizeof(key_type));                                                        \
    memset((u8 *)value, 0, sizeof(value_type));                                                    \
  }

#define HashMapTemplate(type_key, type_value, hash_func, key_equals_func, key_is_empty_func)       \
  EmptyKeyValueFuncTemplate(type_key, type_value)                                                  \
      HashMapTemplateFull(type_key, type_value, HashMap_##type_key##To##type_value,                \
                          HashMap_##type_key##To##type_value##_, hash_func, key_equals_func,       \
                          key_is_empty_func, EmptyKeyValueDefault_##key_type##_##value_type, u64)

#define HashMapTemplateFull(type_key, type_value, struct_name, funcs_prefix, hash_func,            \
                            key_equals_func, key_is_empty_func, empty_key_value_func, index_type)  \
  typedef struct                                                                                   \
  {                                                                                                \
    type_key   *keys;                                                                              \
    type_value *values;                                                                            \
    index_type  capacity;                                                                          \
  } struct_name;                                                                                   \
                                                                                                   \
  internal struct_name funcs_prefix##Init(Allocator allocator, index_type capacity)                \
  {                                                                                                \
    Assert(capacity > 0);                                                                          \
    struct_name res;                                                                               \
    res.capacity = capacity;                                                                       \
    res.keys     = Alloc(type_key, res.capacity);                                                  \
    res.values   = Alloc(type_value, res.capacity);                                                \
    if (!res.keys || !res.values)                                                                  \
    {                                                                                              \
      res.capacity = 0;                                                                            \
    }                                                                                              \
    return res;                                                                                    \
  }                                                                                                \
                                                                                                   \
  internal void funcs_prefix##Deinit(Allocator allocator, struct_name *map)                        \
  {                                                                                                \
    if (map && map->capacity > 0)                                                                  \
    {                                                                                              \
      Free(map->keys, map->capacity);                                                              \
      Free(map->values, map->capacity);                                                            \
      map->capacity = 0;                                                                           \
    }                                                                                              \
  }                                                                                                \
                                                                                                   \
  internal void funcs_prefix##Reset(struct_name *map)                                              \
  {                                                                                                \
    memset(map->keys, 0, sizeof(String) * map->capacity);                                          \
    memset(map->values, 0, sizeof(i64) * map->capacity);                                           \
  }                                                                                                \
                                                                                                   \
  internal type_value *funcs_prefix##Find(struct_name *map, type_key key)                          \
  {                                                                                                \
    type_value *res  = {0};                                                                        \
    index_type  hash = hash_func(key, map->capacity);                                              \
    for (index_type i = hash; i < map->capacity; i += 1)                                           \
    {                                                                                              \
      if (key_equals_func(map->keys[i], key))                                                      \
      {                                                                                            \
        res = &map->values[i];                                                                     \
        break;                                                                                     \
      }                                                                                            \
    }                                                                                              \
    return res;                                                                                    \
  }                                                                                                \
                                                                                                   \
  internal bool funcs_prefix##Remove(struct_name *map, type_key key)                               \
  {                                                                                                \
    bool       removed = false;                                                                    \
    index_type hash    = hash_func(key, map->capacity);                                            \
    for (index_type i = hash; i < map->capacity; i += 1)                                           \
    {                                                                                              \
      if (key_equals_func(map->keys[i], key))                                                      \
      {                                                                                            \
        empty_key_value_func(&map->keys[i], &map->values[i]);                                      \
        removed = true;                                                                            \
        break;                                                                                     \
      }                                                                                            \
    }                                                                                              \
    return removed;                                                                                \
  }                                                                                                \
                                                                                                   \
  internal type_value *funcs_prefix##Push(Allocator allocator, struct_name *map, type_key key,     \
                                          type_value value)                                        \
  {                                                                                                \
    Assert(map);                                                                                   \
    type_value *res = funcs_prefix##Find(map, key);                                                \
    if (res)                                                                                       \
    {                                                                                              \
      *res = value;                                                                                \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
      index_type hash = hash_func(key, map->capacity);                                             \
      if (key_is_empty_func(map->keys[hash]))                                                      \
      {                                                                                            \
        map->keys[hash]   = key;                                                                   \
        map->values[hash] = value;                                                                 \
        res               = &map->values[hash];                                                    \
      }                                                                                            \
      else                                                                                         \
      {                                                                                            \
        bool inserted = false;                                                                     \
        for (index_type i = hash + 1; i < map->capacity; i += 1)                                   \
        {                                                                                          \
          if (key_is_empty_func(map->keys[i]))                                                     \
          {                                                                                        \
            map->keys[i]   = key;                                                                  \
            map->values[i] = value;                                                                \
            res            = &map->values[i];                                                      \
            inserted       = true;                                                                 \
            break;                                                                                 \
          }                                                                                        \
        }                                                                                          \
        if (!inserted)                                                                             \
        {                                                                                          \
          struct_name new_map = funcs_prefix##Init(allocator, map->capacity * 2);                  \
          for (index_type i = 0; i < map->capacity; i += 1)                                        \
          {                                                                                        \
            if (!key_is_empty_func(map->keys[i]))                                                  \
            {                                                                                      \
              funcs_prefix##Push(allocator, &new_map, map->keys[i], map->values[i]);               \
            }                                                                                      \
          }                                                                                        \
          funcs_prefix##Deinit(allocator, map);                                                    \
          *map = new_map;                                                                          \
                                                                                                   \
          res = funcs_prefix##Push(allocator, map, key, value);                                    \
        }                                                                                          \
      }                                                                                            \
    }                                                                                              \
    return res;                                                                                    \
  }                                                                                                \
                                                                                                   \
  typedef struct                                                                                   \
  {                                                                                                \
    type_key   key;                                                                                \
    type_value value;                                                                              \
  } Pair_##type_key##To##type_value;                                                               \
  ArrayTemplate(Pair_##type_key##To##type_value);                                                  \
                                                                                                   \
  internal ArrayPair_##type_key##To##type_value funcs_prefix##KeyValuePairs(Allocator   allocator, \
                                                                            struct_name map)       \
  {                                                                                                \
    ArrayPair_##type_key##To##type_value pairs =                                                   \
        ArrayPair_##type_key##To##type_value##_Init(allocator, map.capacity);                      \
    for (index_type i = 0; i < map.capacity; i += 1)                                               \
    {                                                                                              \
      if (!key_is_empty_func(map.keys[i]))                                                         \
      {                                                                                            \
        Pair_##type_key##To##type_value p = {0};                                                   \
        p.key                             = map.keys[i];                                           \
        p.value                           = map.values[i];                                         \
        ArrayPair_##type_key##To##type_value##_Push(allocator, &pairs, p);                         \
      }                                                                                            \
    }                                                                                              \
    return pairs;                                                                                  \
  }

#endif