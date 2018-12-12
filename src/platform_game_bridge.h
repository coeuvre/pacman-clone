#ifndef PLATFORM_GAME_BRIDGE_H
#define PLATFORM_GAME_BRIDGE_H

#include "defs.h"

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
typedef void *platform_allocate_memory_fn(size_t Size);
typedef void *platform_reallocate_memory_fn(void *Pointer, size_t NewSize);
typedef void platform_deallocate_memory_fn(void *Pointer);

typedef void *platform_read_entire_file(const char *URL, size_t *FileSize);

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

#define GAME_LOAD(name) void name(const platform *Platform)
typedef GAME_LOAD(game_load_fn);

#define GAME_INIT(name) void *name(const platform *Platform)
typedef GAME_INIT(game_init_fn);

#define GAME_UPDATE(name) void name(void *GameState, const platform *Platform, const input *Input)
typedef GAME_UPDATE(game_update_fn);

#define GAME_RENDER(name) void name(void *GameState, const platform *Platform)
typedef GAME_RENDER(game_render_fn);

#endif // PLATFORM_GAME_BRIDGE_H
