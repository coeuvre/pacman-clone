#ifndef GAME_H
#define GAME_H

#include "memory_api.h"
#include "file_api.h"
#include "renderer_api.h"

struct game_required_api
{
    memory_api Memory;
    file_api File;
    renderer_api Renderer;
};

struct input
{
    float DeltaTime;
};

#define GAME_LOAD(name) void name(const game_required_api *Api)
typedef GAME_LOAD(game_load_fn);

#define GAME_INIT(name) void *name(renderer_context *RendererContext)
typedef GAME_INIT(game_init_fn);

#define GAME_UPDATE(name) void name(void *GameState, const input *Input)
typedef GAME_UPDATE(game_update_fn);

#define GAME_RENDER(name) void name(void *GameState, renderer_context *RendererContext)
typedef GAME_RENDER(game_render_fn);

#endif // GAME_H
