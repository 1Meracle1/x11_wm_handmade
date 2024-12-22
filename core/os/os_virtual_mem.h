#ifndef OS_VIRTUAL_H
#define OS_VIRTUAL_H

#include "os_defines.h"
#include "../memory/memory.h"

internal void           *OS_Reserve(u64 size, AllocationError *err);
internal AllocationError OS_Commit(void *data, u64 size);
internal void            OS_Decommit(void *data, u64 size);
internal void            OS_Release(void *data, u64 size);

internal u64 OS_PageSize();

#endif