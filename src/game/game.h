#ifndef GAME_GAME_H
#define GAME_GAME_H

#include "platform/platform.h"
#include "renderer/renderer.h"

struct game_required_module
{
    memory_module *Memory;
    file_module *File;
    renderer_module *Renderer;
};

struct input
{
    float DeltaTime;
};

#define GAME_ADVANCE(name) void name(void *GameState, renderer_context *RendererContext, const input *Input)
typedef GAME_ADVANCE(game_advance_fn);

struct game_module
{
    void *GameState;
    game_advance_fn *Advance;
};

#define GAME_LOAD(name) void name(game_module *Game, const game_required_module *Module)
typedef GAME_LOAD(game_load_fn);

#endif // GAME_GAME_H
