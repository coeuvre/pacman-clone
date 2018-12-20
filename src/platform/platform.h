#ifndef PLATFORM_PLATFORM_H
#define PLATFORM_PLATFORM_H

#include "core/defs.h"


#define MEMORY_ALLOCATE(name) void *name(size_t Size)
typedef MEMORY_ALLOCATE(memory_allocate_fn);

#define MEMORY_REALLOCATE(name) void *name(void *Pointer, size_t NewSize)
typedef MEMORY_REALLOCATE(memory_reallocate_fn);

#define MEMORY_DEALLOCATE(name) void name(void *Pointer)
typedef MEMORY_DEALLOCATE(memory_deallocate_fn);

struct memory_module
{
    memory_allocate_fn *Allocate;
    memory_reallocate_fn *Reallocate;
    memory_deallocate_fn *Deallocate;
};


#define FILE_READ_ENTIRE_FILE(name) void *name(const char *URL, size_t *FileSize)
typedef FILE_READ_ENTIRE_FILE(file_read_entire_file_fn);

struct file_module
{
    file_read_entire_file_fn *ReadEntireFile;
};


#endif // PLATFORM_PLATFORM_H
