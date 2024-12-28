#ifndef CORE_CHANNEL_H
#define CORE_CHANNEL_H

#include "../memory/memory.h"
#include "string.h"
#include <pthread.h>

internal AllocationError StringClone(Allocator allocator, String src, String *dst)
{
  AllocationError err = AllocationError_None;
  if (!dst)
  {
    err = AllocationError_InvalidPointer;
  }
  else if (src.size)
  {
    dst->data = Alloc(u8, src.size);
    if (dst->data)
    {
      memcpy(dst->data, src.data, src.size);
      dst->size = src.size;
    }
    else
    {
      err = AllocationError_OutOfMemory;
    }
  }
  return err;
}

typedef struct
{
  Allocator allocator;
  // Mutex for synchronization
  pthread_mutex_t mutex;
  // Condition variable for a receiver thread to wait till queue becomes non-empty
  pthread_cond_t not_empty;
  // Condition variable for a sender thread to wait till queue becomes empty
  pthread_cond_t empty;
  ArrayString    array;
  bool           closed;
} Channel;

internal Channel ChannelInit(Allocator allocator, u64 capacity)
{
  Channel channel   = {0};
  channel.allocator = allocator;
  channel.array     = ArrayString_Init(allocator, capacity);
  pthread_mutex_init(&channel.mutex, NULL);
  pthread_cond_init(&channel.not_empty, NULL);
  pthread_cond_init(&channel.empty, NULL);
  return channel;
}

internal void ChannelDeinit(Channel *channel)
{
  ArrayString_Deinit(channel->allocator, &channel->array);
  pthread_mutex_destroy(&channel->mutex);
  pthread_cond_destroy(&channel->not_empty);
}

internal bool ChannelSend(Channel *channel, String item)
{
  bool ok = false;
  pthread_mutex_lock(&channel->mutex);
  if (!channel->closed)
  {
    AllocationError alloc_err = ArrayString_Push(channel->allocator, &channel->array, item);
    if (alloc_err == AllocationError_None)
    {
      pthread_cond_signal(&channel->not_empty);
      ok = true;
    }
  }
  pthread_mutex_unlock(&channel->mutex);
  return ok;
}

internal bool ChannelSendCloned(Channel *channel, String item)
{
  bool ok = false;
  pthread_mutex_lock(&channel->mutex);
  if (!channel->closed)
  {
    String          cloned_item;
    AllocationError alloc_err = StringClone(channel->allocator, item, &cloned_item);
    if (alloc_err == AllocationError_None)
    {
      alloc_err = ArrayString_Push(channel->allocator, &channel->array, cloned_item);
    }
    if (alloc_err == AllocationError_None)
    {
      pthread_cond_signal(&channel->not_empty);
      ok = true;
    }
  }
  pthread_mutex_unlock(&channel->mutex);
  return ok;
}

internal bool ChannelReceive(Channel *channel, String *item)
{
  bool ok = false;
  pthread_mutex_lock(&channel->mutex);
  if (!channel->closed)
  {
    while (channel->array.size == 0 && !channel->closed)
    {
      pthread_cond_wait(&channel->not_empty, &channel->mutex);
    }
    if (channel->array.size == 0 && channel->closed)
    {
      pthread_mutex_unlock(&channel->mutex);
    }
    else
    {
      *item = channel->array.data[0];
      ArrayString_UnorderedRemove(&channel->array, 0);
      ok = true;
    }
  }
  pthread_mutex_unlock(&channel->mutex);
  return ok;
}

internal bool ChannelClose(Channel *channel)
{
  bool ok = false;
  pthread_mutex_lock(&channel->mutex);
  if (!channel->closed)
  {
    channel->closed = true;
    ok              = true;
    // Wake up all waiting threads
    pthread_cond_broadcast(&channel->not_empty);
  }
  pthread_mutex_unlock(&channel->mutex);
  return ok;
}

#endif