#ifndef GAME_GAME_H
#define GAME_GAME_H

#include "platform/platform.h"
#include "renderer/renderer.h"

struct input
{
    float DeltaTime;
};

struct game_dependencies
{
    // Memory
    allocate_memory_fn *AllocateMemory;
    reallocate_memory_fn *ReallocateMemory;
    deallocate_memory_fn *DeallocateMemory;

    // File
    read_entire_file_fn *ReadEntireFile;

    // Renderer
    render_context *RenderContext;
    load_texture_fn *LoadTexture;

    input *Input;
};

#define INIT_GAME_STATE(name) void *name()

typedef INIT_GAME_STATE(init_game_state_fn);

#define UPDATE_GAME_STATE(name) void name(void *GameState)

typedef UPDATE_GAME_STATE(update_game_state_fn);

struct game_module
{
    init_game_state_fn *InitGameState;
    update_game_state_fn *UpdateGameState;
};

#define INIT_GAME_MODULE(name) game_module name(const game_dependencies *Dependencies)

typedef INIT_GAME_MODULE(game_load_fn);

#endif // GAME_GAME_H
