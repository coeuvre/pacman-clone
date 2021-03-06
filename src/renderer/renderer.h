#ifndef RENDERER_API_H
#define RENDERER_API_H

typedef uint64_t texture_handle;

struct texture
{
    uint32_t ReferenceCount;
    uint32_t Width;
    uint32_t Height;
    texture_handle Handle;
};

inline texture *
IncreaseTextureReferenceCount(texture *Texture)
{
    ++Texture->ReferenceCount;
    return Texture;
}

inline void
DecreaseTextureReferenceCount(texture *Texture)
{
    Assert(Texture->ReferenceCount > 1);
    --Texture->ReferenceCount;
}

enum render_command_data_type
{
    RenderCommandDataType_textured_rect2 = 1
};

struct textured_rect2
{
    rect2 DstRect;
    texture *Texture;
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

static render_context *GetRenderContext();

// Assuming the color is in sRGB space and is NOT pre-multiply by alpha.
//
// ChannelsPerPixel
//     4 - RGBA8 format
static texture *LoadTexture(uint32_t Width, uint32_t Height, uint32_t ChannelsPerPixel, int32_t Pitch, uint8_t *Bytes);
static void UnloadTexture(texture *Texture);

static render_command_buffer *BeginRenderCommand();
static void EndRenderCommand(render_command_buffer *CommandBuffer);
static void PushTexturedRectangle2(render_command_buffer *CommandBuffer, rect2 *DstRect, texture *Texture, rect2 *SrcRect);

#endif // RENDERER_API_H
