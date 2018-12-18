#include "platform.h"

#include <stdio.h>

platform GlobalPlatform = {};
renderer GlobalRenderer = {};

#define STBI_ONLY_PNG
#define STBI_MALLOC(Size) PlatformAllocateMemory(Size)
#define STBI_REALLOC(Pointer, NewSize) PlatformReallocateMemory(Pointer, NewSize)
#define STBI_FREE(Pointer) PlatformDeallocateMemory(Pointer)
#define STB_IMAGE_IMPLEMENTATION

#include <stb/stb_image.h>

#include "renderer.cpp"

struct bitmap
{
    int Width;
    int Height;
    int ChannelsPerPixel;
    uint8_t *Bytes;
};

struct game_state
{
    float Counter;
    renderer_texture *TestTexture;
};

static bitmap *
LoadBitmap(const char *URL)
{
    bitmap *Result = 0;

    size_t FileSize;
    void *FileContent = PlatformReadEntireFile(URL, &FileSize);
    if (FileContent)
    {
        Result = (bitmap *) PlatformAllocateMemory(sizeof(*Result));
        Result->Bytes = stbi_load_from_memory((const uint8_t *) FileContent, (int) FileSize,
                                              &Result->Width, &Result->Height, 0, 4);
        Result->ChannelsPerPixel = 4;

        PlatformDeallocateMemory(FileContent);
    }
    return Result;
}

static void
UnloadBitmap(bitmap **Bitmap)
{
    stbi_image_free((*Bitmap)->Bytes);
    PlatformDeallocateMemory(*Bitmap);
    *Bitmap = 0;
}

static renderer_texture *
LoadTexture(renderer_context *Context, const char *URL)
{
    renderer_texture *Result = 0;

    bitmap *TestBitmap = LoadBitmap(URL);
    if (TestBitmap)
    {
        Result = RendererLoadTexture(Context, TestBitmap->Width, TestBitmap->Height, TestBitmap->ChannelsPerPixel,
                                     TestBitmap->Bytes);
        UnloadBitmap(&TestBitmap);
    }

    return Result;
}

static void
Init(game_state *GameState, renderer_context *RendererContext)
{
    GameState->TestTexture = LoadTexture(RendererContext, "assets://test.png");
}

static void
Update(game_state *GameState, const input *Input)
{
    GameState->Counter += Input->DeltaTime;
}

static void
Render(game_state *GameState, renderer_context *RendererContext)
{
    if (GameState->TestTexture)
    {
        RendererRenderTexturedQuad2(RendererContext, 0, GameState->TestTexture, 0);
    }
}

extern "C"
{

EXPORT GAME_LOAD(GameLoad)
{
    GlobalPlatform = *Platform;
    GlobalRenderer = *Renderer;
}

EXPORT GAME_INIT(GameInit)
{
    game_state *GameState = (game_state *) PlatformAllocateMemory(sizeof(*GameState));

    Init(GameState, RendererContext);

    return GameState;
}

EXPORT GAME_UPDATE(GameUpdate)
{
    Update((game_state *) GameState, Input);
}

EXPORT GAME_RENDER(GameRender)
{
    Render((game_state *) GameState, RendererContext);
}

}
