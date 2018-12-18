#ifndef RENDERER_H
#define RENDERER_H

#include "game_math.h"

struct renderer_context;

typedef uint64_t texture_handle;

struct renderer_texture
{
    int Width;
    int Height;
    texture_handle Handle;
};

#define RENDERER_LOAD_TEXTURE(name) renderer_texture *name(renderer_context *Context, int Width, int Height, int ChannelsPerPixel, uint8_t *Bytes)
typedef RENDERER_LOAD_TEXTURE(renderer_load_texture_fn);

struct renderer
{
    renderer_load_texture_fn *LoadTexture;
};

extern renderer GlobalRenderer;

inline RENDERER_LOAD_TEXTURE(RendererLoadTexture)
{
    return GlobalRenderer.LoadTexture(Context, Width, Height, ChannelsPerPixel, Bytes);
}

enum renderer_command_type
{
    RendererCommandType_render_textured_quad = 1
};

struct renderer_command_render_texture
{
    rect2 DestRect;
    renderer_texture *Texture;
    rect2 SourceRect;
};

struct renderer_command_header
{
    renderer_command_type Type;
};

struct renderer_context
{
    void *Renderer;

    uint32_t CommandBufferSize;
    uint8_t *CommandBufferBase;
    uint8_t *CommandBufferAt;

    uint32_t ViewportWidth;
    uint32_t ViewportHeight;
};

#endif // RENDERER_H
