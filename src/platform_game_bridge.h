#ifndef PLATFORM_GAME_BRIDGE_H
#define PLATFORM_GAME_BRIDGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

typedef void *platform_allocate_fn(usize Size);
typedef void platform_deallocate_fn(void *Ptr);

typedef struct platform {
    platform_allocate_fn *Allocate;
    platform_deallocate_fn *Deallocate;

    void *GameState;
} platform;


typedef void update_and_render_game_fn(platform *Platform);

#ifdef __cplusplus
}
#endif

#endif // PLATFORM_GAME_BRIDGE_H
