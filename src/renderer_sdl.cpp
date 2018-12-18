#include "renderer.h"

#include <SDL2/SDL.h>

renderer GlobalRenderer = {};

static RENDERER_LOAD_TEXTURE(RendererSDLLoadTexture)
{
    Assert(ChannelsPerPixel == 4);

    SDL_Renderer *Renderer = (SDL_Renderer *) Context->Renderer;

    renderer_texture *Result = (renderer_texture *) PlatformAllocateMemory(sizeof(*Result));
    Result->Width = Width;
    Result->Height = Height;
    SDL_Surface *Surface = SDL_CreateRGBSurfaceWithFormatFrom(Bytes, Width, Height, 32, ChannelsPerPixel * Width,
                                                              SDL_PIXELFORMAT_RGBA8888);
    SDL_Texture *Texture = SDL_CreateTextureFromSurface(Renderer, Surface);
    Result->Handle = (uint64_t) Texture;
    SDL_FreeSurface(Surface);

    return Result;
}

static void
RendererSDLInit(renderer_context *Context, SDL_Window *Window, uint32_t ViewportWidth, uint32_t ViewportHeight)
{
    GlobalRenderer.LoadTexture = &RendererSDLLoadTexture;

    Context->Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    Context->CommandBufferSize = KB(4);
    Context->CommandBufferBase = (uint8_t *) PlatformAllocateMemory(Context->CommandBufferSize);
    Context->CommandBufferAt = Context->CommandBufferBase;

    Context->ViewportWidth = ViewportWidth;
    Context->ViewportHeight = ViewportHeight;
}

static void
RendererSDLBeginFrame(renderer_context *Context)
{
    SDL_RenderClear((SDL_Renderer *) Context->Renderer);

    Context->CommandBufferAt = Context->CommandBufferBase;
}

static void
RendererSDLEndFrame(renderer_context *Context)
{
    SDL_Renderer *Renderer = (SDL_Renderer *) Context->Renderer;

    for (uint8_t *At = Context->CommandBufferBase; At < Context->CommandBufferAt;)
    {
        renderer_command_header *Header = (renderer_command_header *) At;
        At += sizeof(*Header);

        switch (Header->Type)
        {
            case RendererCommandType_render_textured_quad:
            {
                renderer_command_render_texture *Data = (renderer_command_render_texture *) At;
                At += sizeof(*Data);

                SDL_Texture *Texture = (SDL_Texture *) Data->Texture->Handle;
                SDL_RenderCopy(Renderer, Texture, 0, 0);
                break;
            }

            default: INVALID_CODE_PATH;
        }
    }

    SDL_RenderPresent((SDL_Renderer *) Context->Renderer);
}
