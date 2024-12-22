#include "os_virtual_mem.h"

#ifdef OS_LINUX

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

extern int getpagesize(void);
extern int madvise(void *__addr, size_t __len, int __advice);

internal void *OS_Reserve(u64 size, AllocationError *err)
{
  u8 *mapped = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (mapped == MAP_FAILED)
  {
    if (err != NULL)
    {
      if (errno == ENOMEM)
      {
        *err = AllocationError_OutOfMemory;
      }
      else if (errno == EINVAL)
      {
        *err = AllocationError_InvalidArgument;
      }
    }
    return NULL;
  }
  return mapped;
}

internal AllocationError OS_Commit(void *data, u64 size)
{
  mprotect(data, size, PROT_READ | PROT_WRITE);
  if (errno == EINVAL)
  {
    return AllocationError_InvalidPointer;
  }
  else if (errno == ENOMEM)
  {
    return AllocationError_OutOfMemory;
  }
  return AllocationError_None;
}

internal void OS_Decommit(void *data, u64 size)
{
  mprotect(data, size, PROT_NONE);
#define _MADV_FREE 8
  madvise(data, size, _MADV_FREE);
}

internal void OS_Release(void *data, u64 size)
{
  munmap(data, size);
}

internal u64 OS_PageSize()
{
  return (u64)getpagesize();
}

#else
#error Virtual memory OS allocation interface is not implemented on this platform
#endif