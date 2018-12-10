#ifndef PLATFORM_GAME_BRIDGE_H
#define PLATFORM_GAME_BRIDGE_H

#ifdef __cplusplus
extern "C" {
#endif

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
typedef void platform_deallocate_memory_fn(void *Pointer);

typedef struct platform
{
//    platform_log_fn *Log;
    platform_allocate_memory_fn *AllocateMemory;
    platform_deallocate_memory_fn *DeallocateMemory;

    void *GameState;
} platform;

typedef struct game_input
{
    float DeltaTime;
} game_input;

#define GAME_INIT(name) void name(platform *Platform)
typedef GAME_INIT(game_init_fn);

#define GAME_UPDATE(name) void name(game_input *Input)
typedef GAME_UPDATE(game_update_fn);

#define GAME_RENDER(name) void name()
typedef GAME_RENDER(game_render_fn);

#ifdef __cplusplus
}
#endif

#endif // PLATFORM_GAME_BRIDGE_H
