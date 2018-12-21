#ifndef RENDERER_API_H
#define RENDERER_API_H

#include "core/defs.h"
#include "core/math.h"

typedef uint64_t texture_handle;

struct texture
{
    int Width;
    int Height;
    texture_handle Handle;
};

enum renderer_command_type
{
    RendererCommandType_render_textured_quad = 1
};

struct renderer_command_render_texture
{
    rect2 DestRect;
    texture *Texture;
    rect2 SourceRect;
};

struct renderer_command_header
{
    renderer_command_type Type;
};

struct render_context
{
    uint32_t CommandBufferSize;
    uint8_t *CommandBufferBase;
    uint8_t *CommandBufferAt;

    uint32_t ViewportWidth;
    uint32_t ViewportHeight;
};

#define LOAD_TEXTURE(name) texture *name(int Width, int Height, int ChannelsPerPixel, uint8_t *Bytes)
typedef LOAD_TEXTURE(load_texture_fn);

#endif // RENDERER_API_H
