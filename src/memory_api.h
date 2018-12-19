#ifndef MEMORY_API_H
#define MEMORY_API_H

#include "defs.h"

#define MEMORY_ALLOCATE(name) void *name(size_t Size)
typedef MEMORY_ALLOCATE(memory_allocate_fn);

#define MEMORY_REALLOCATE(name) void *name(void *Pointer, size_t NewSize)
typedef MEMORY_REALLOCATE(memory_reallocate_fn);

#define MEMORY_DEALLOCATE(name) void name(void *Pointer)
typedef MEMORY_DEALLOCATE(memory_deallocate_fn);

struct memory_api
{
    memory_allocate_fn *Allocate;
    memory_reallocate_fn *Reallocate;
    memory_deallocate_fn *Deallocate;
};

#endif // MEMORY_API_H