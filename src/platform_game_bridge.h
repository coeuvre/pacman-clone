#ifndef PLATFORM_GAME_BRIDGE_H
#define PLATFORM_GAME_BRIDGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "defs.h"

typedef void *platform_allocate_memory_fn(size_t Size);
typedef void platform_deallocate_memory_fn(void *Ptr);

typedef struct platform {
    platform_allocate_memory_fn *AllocateMemory;
    platform_deallocate_memory_fn *DeallocateMemory;

    void *GameState;
} platform;


typedef void update_and_render_game_fn(platform *Platform);

#ifdef __cplusplus
}
#endif

#endif // PLATFORM_GAME_BRIDGE_H
