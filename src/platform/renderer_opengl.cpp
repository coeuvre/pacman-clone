#include "renderer/renderer.h"

#include <SDL2/SDL_opengl.h>

static LOAD_TEXTURE(RendererOpenGLLoadTexture)
{
    Assert(ChannelsPerPixel == 4);

    texture *Result = (texture *) AllocateMemory(sizeof(*Result));
    Result->Width = Width;
    Result->Height = Height;
//    Result->Handle = (uint64_t) Texture;

    return Result;
}

static void
RendererOpenGLInit(render_context *Context)
{
    Context->CommandBufferSize = KB(4);
    Context->CommandBufferBase = (uint8_t *) AllocateMemory(Context->CommandBufferSize);
    Context->CommandBufferAt = Context->CommandBufferBase;
}

static void
RendererOpenGLBeginFrame(render_context *Context, uint32_t ViewportWidth, uint32_t ViewportHeight)
{
    Context->CommandBufferAt = Context->CommandBufferBase;
    Context->ViewportWidth = ViewportWidth;
    Context->ViewportHeight = ViewportHeight;
}

static void
RendererOpenGLEndFrame(render_context *Context)
{

}

static void
RendererOpenGLRender(render_context *Context)
{
    for (uint8_t *At = Context->CommandBufferBase; At < Context->CommandBufferAt; NOOP)
    {
        renderer_command_header *Header = (renderer_command_header *) At;
        At += sizeof(*Header);

        switch (Header->Type)
        {
            case RendererCommandType_render_textured_quad:
            {
                renderer_command_render_texture *Data = (renderer_command_render_texture *) At;
                At += sizeof(*Data);

                break;
            }

            default:
                INVALID_CODE_PATH;
        }
    }
}
