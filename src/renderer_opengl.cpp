#include "renderer_api.h"

#include <SDL2/SDL_opengl.h>

struct renderer_opengl_required_api
{
    memory_api Memory;
};

renderer_opengl_required_api GlobalApi;

struct renderer_opengl_context
{

};

static RENDERER_LOAD_TEXTURE(RendererOpenGLLoadTexture)
{
    Assert(ChannelsPerPixel == 4);

    renderer_texture *Result = (renderer_texture *) GlobalApi.Memory.Allocate(sizeof(*Result));
    Result->Width = Width;
    Result->Height = Height;
//    Result->Handle = (uint64_t) Texture;

    return Result;
}

static void
RendererOpenGLInitApi(renderer_api *Renderer)
{
    Renderer->LoadTexture = &RendererOpenGLLoadTexture;
}

static void
RendererOpenGLInit(renderer_opengl_required_api *Api, renderer_context *Context)
{
    GlobalApi = *Api;

    Context->CommandBufferSize = KB(4);
    Context->CommandBufferBase = (uint8_t *) GlobalApi.Memory.Allocate(Context->CommandBufferSize);
    Context->CommandBufferAt = Context->CommandBufferBase;
}

static void
RendererOpenGLBeginFrame(renderer_context *Context, uint32_t ViewportWidth, uint32_t ViewportHeight)
{
    Context->CommandBufferAt = Context->CommandBufferBase;
    Context->ViewportWidth = ViewportWidth;
    Context->ViewportHeight = ViewportHeight;
}

static void
RendererOpenGLEndFrame(renderer_context *Context)
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

            default: INVALID_CODE_PATH;
        }
    }
}
