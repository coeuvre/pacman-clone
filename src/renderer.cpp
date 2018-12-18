#include "renderer.h"

static void
RendererRenderTexturedQuad2(renderer_context *Context, rect2 *DestRect, renderer_texture *Texture, rect2 *SourceRect)
{
    uint32_t HeaderSize = sizeof(renderer_command_type);
    uint32_t DataSize = sizeof(renderer_command_render_texture);
    uint32_t Size = HeaderSize + DataSize;
    // TODO: Dynamically resize
    Assert(Texture && Context->CommandBufferAt - Context->CommandBufferBase + Size < Context->CommandBufferSize);

    renderer_command_header *Header = (renderer_command_header *) Context->CommandBufferAt;
    Context->CommandBufferAt += HeaderSize;

    renderer_command_render_texture *Data = (renderer_command_render_texture *) Context->CommandBufferAt;
    Context->CommandBufferAt += DataSize;

    Header->Type = RendererCommandType_render_textured_quad;
    if (DestRect)
    {
        Data->DestRect = *DestRect;
    }
    else
    {
        Data->DestRect = Rect2MinMax(0, 0, Context->ViewportWidth, Context->ViewportHeight);
    }
    Data->Texture = Texture;
    if (SourceRect)
    {
        Data->SourceRect = *SourceRect;
    }
    else
    {
        Data->SourceRect = Rect2MinMax(0, 0, Texture->Width, Texture->Height);
    }
}
