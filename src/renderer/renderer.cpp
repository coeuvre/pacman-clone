#include "renderer/renderer.h"

static void *
PushRenderCommandRaw(render_command_buffer *CommandBuffer, render_command_data_type Type, size_t DataSize)
{
    size_t HeaderSize = sizeof(render_command_data_type);
    size_t Size = HeaderSize + DataSize;
    // TODO: Dynamically resize
    Assert(CommandBuffer->At - CommandBuffer->Base + Size < CommandBuffer->Size);

    render_command_header *Header = (render_command_header *) CommandBuffer->At;
    CommandBuffer->At += HeaderSize;

    Header->Type = Type;

    void *Data = CommandBuffer->At;
    CommandBuffer->At += DataSize;

    return Data;
}

#define PushRenderCommand(CommandBuffer, Type) (Type *) \
    PushRenderCommandRaw(CommandBuffer, RenderCommandDataType_##Type, sizeof(Type))

static void
PushTexturedRectangle2(render_command_buffer *CommandBuffer, rect2 *DstRect, texture *Texture, rect2 *SrcRect)
{
    Assert(Texture);

    render_context *RenderContext = GetRenderContext();
    textured_rect2 *Data = PushRenderCommand(CommandBuffer, textured_rect2);

    if (DstRect)
    {
        Data->DstRect = *DstRect;
    }
    else
    {
        Data->DstRect = Rectangle2MinMax(0, 0, RenderContext->ViewportWidth, RenderContext->ViewportHeight);
    }
    Data->Texture = IncreaseTextureReferenceCount(Texture);
    if (SrcRect)
    {
        Data->SrcRect = *SrcRect;
    }
    else
    {
        Data->SrcRect = Rectangle2MinMax(0, 0, Texture->Width, Texture->Height);
    }
}

static void
InitRenderCommandBuffer(render_command_buffer *CommandBuffer)
{
    CommandBuffer->Size = KB(4);
    CommandBuffer->Base = (uint8_t *) AllocateMemory(CommandBuffer->Size);
    CommandBuffer->At = CommandBuffer->Base;
}

static render_command_buffer *
BeginRenderCommand()
{
    render_context *RenderContext = GetRenderContext();
    render_command_buffer *CommandBuffer = &RenderContext->CommandBuffer;

    if (!CommandBuffer->Base)
    {
        InitRenderCommandBuffer(CommandBuffer);
    }

    CommandBuffer->At = CommandBuffer->Base;

    return CommandBuffer;
}

static void
EndRenderCommand(render_command_buffer *CommandBuffer)
{

}
