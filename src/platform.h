#ifndef PLATFORM_H
#define PLATFORM_H

#include "defs.h"

//
// Functions that platform provides for game
//

//typedef enum platform_log_level
//{
//    PlatformLogLevel_Verbose = 1,
//    PlatformLogLevel_Debug,
//    PlatformLogLevel_Info,
//    PlatformLogLevel_Warn,
//    PlatformLogLevel_Error,
//    PlatformLogLevel_Critical,
//    PlatformLogLevelCount,
//} platform_log_level;

//typedef void platform_log_fn(int Level, const char *FormatString, ...);

#define PLATFORM_ALLOCATE_MEMORY(name) void *name(size_t Size)
typedef PLATFORM_ALLOCATE_MEMORY(platform_allocate_memory_fn);

#define PLATFORM_REALLOCATE_MEMORY(name) void *name(void *Pointer, size_t NewSize)
typedef PLATFORM_REALLOCATE_MEMORY(platform_reallocate_memory_fn);

#define PLATFORM_DEALLOCATE_MEMORY(name) void name(void *Pointer)
typedef PLATFORM_DEALLOCATE_MEMORY(platform_deallocate_memory_fn);

#define PLATFORM_READ_ENTIRE_FILE(name) void *name(const char *URL, size_t *FileSize)
typedef PLATFORM_READ_ENTIRE_FILE(platform_read_entire_file);

struct platform
{
//    platform_log_fn *Log;
    platform_allocate_memory_fn *AllocateMemory;
    platform_reallocate_memory_fn *ReallocateMemory;
    platform_deallocate_memory_fn *DeallocateMemory;

    platform_read_entire_file *ReadEntireFile;
};

struct input
{
    float DeltaTime;
};

//
// Functions that platform will call into game
//

#define GAME_LOAD(name) void name(const platform *Platform)
typedef GAME_LOAD(game_load_fn);

#define GAME_INIT(name) void *name()
typedef GAME_INIT(game_init_fn);

#define GAME_UPDATE(name) void name(void *GameState, const input *Input)
typedef GAME_UPDATE(game_update_fn);
#define GAME_RENDER(name) void name(void *GameState)
typedef GAME_RENDER(game_render_fn);

#endif // PLATFORM_H
