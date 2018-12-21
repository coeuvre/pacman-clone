#ifndef PLATFORM_PLATFORM_H
#define PLATFORM_PLATFORM_H

#include "core/defs.h"

#define ALLOCATE_MEMORY(name) void *name(size_t Size)
typedef ALLOCATE_MEMORY(allocate_memory_fn);

#define REALLOCATE_MEMORY(name) void *name(void *Pointer, size_t NewSize)
typedef REALLOCATE_MEMORY(reallocate_memory_fn);

#define DEALLOCATE_MEMORY(name) void name(void *Pointer)
typedef DEALLOCATE_MEMORY(deallocate_memory_fn);

#define READ_ENTIRE_FILE(name) void *name(const char *URL, size_t *FileSize)
typedef READ_ENTIRE_FILE(read_entire_file_fn);


#endif // PLATFORM_PLATFORM_H
