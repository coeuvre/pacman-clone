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

enum render_command_data_type
{
    RenderCommandDataType_textured_rect2 = 1
};

struct textured_rect2
{
    rect2 DstRect;
    texture Texture;
    rect2 SrcRect;
};

struct render_command_header
{
    render_command_data_type Type;
};

struct render_command_buffer
{
    size_t Size;
    uint8_t *Base;
    uint8_t *At;
};

struct render_context
{
    render_command_buffer CommandBuffer;
    uint32_t ViewportWidth;
    uint32_t ViewportHeight;
};

#define GET_RENDER_CONTEXT(name) render_context *name();
typedef GET_RENDER_CONTEXT(get_render_context_fn);

#define LOAD_TEXTURE(name) texture *name(uint32_t Width, uint32_t Height, uint32_t ChannelsPerPixel, uint8_t *Bytes)
typedef LOAD_TEXTURE(load_texture_fn);

#endif // RENDERER_API_H
