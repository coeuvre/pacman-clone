#ifndef GAME_GAME_H
#define GAME_GAME_H

#include "platform/platform.h"
#include "renderer/renderer.h"

struct game_dependencies
{
    // Memory
    allocate_memory_fn *AllocateMemory;
    reallocate_memory_fn *ReallocateMemory;
    deallocate_memory_fn *DeallocateMemory;

    // File
    read_entire_file_fn *ReadEntireFile;

    // Renderer
    get_render_context_fn *GetRenderContext;
    load_texture_fn *LoadTexture;
    unload_texture_fn *UnloadTexture;

    // Input
    get_input_fn *GetInput;
};

struct game_state;

#define UPDATE_GAME(name) void name(game_state *GameState)
typedef UPDATE_GAME(update_game_fn);

struct game_module
{
    update_game_fn *UpdateGame;
};

#define INIT_GAME_MODULE(name) game_module name(const game_dependencies *Dependencies, game_state **GameStatePtr)
typedef INIT_GAME_MODULE(game_load_fn);

#endif // GAME_GAME_H
