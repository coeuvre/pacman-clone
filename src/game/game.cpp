#include "game.h"

static memory_module GlobalMemory;
static file_module GlobalFile;
static renderer_module GlobalRenderer;

#include <stdio.h>

#define STBI_ONLY_PNG
#define STBI_MALLOC(Size) GlobalMemory.Allocate(Size)
#define STBI_REALLOC(Pointer, NewSize) GlobalMemory.Reallocate(Pointer, NewSize)
#define STBI_FREE(Pointer) GlobalMemory.Deallocate(Pointer)
#define STB_IMAGE_IMPLEMENTATION

#include <stb/stb_image.h>

#include "renderer/renderer.cpp"

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
    void *FileContent = GlobalFile.ReadEntireFile(URL, &FileSize);
    if (FileContent)
    {
        Result = (bitmap *) GlobalMemory.Allocate(sizeof(*Result));
        Result->Bytes = stbi_load_from_memory((const uint8_t *) FileContent, (int) FileSize,
                                              &Result->Width, &Result->Height, 0, 4);
        Result->ChannelsPerPixel = 4;

        GlobalMemory.Deallocate(FileContent);
    }
    return Result;
}

static void
UnloadBitmap(bitmap **Bitmap)
{
    stbi_image_free((*Bitmap)->Bytes);
    GlobalMemory.Deallocate(*Bitmap);
    *Bitmap = 0;
}

static renderer_texture *
LoadTexture(const char *URL)
{
    renderer_texture *Result = 0;

    bitmap *TestBitmap = LoadBitmap(URL);
    if (TestBitmap)
    {
        Result = GlobalRenderer.LoadTexture(TestBitmap->Width, TestBitmap->Height,
                                            TestBitmap->ChannelsPerPixel,
                                            TestBitmap->Bytes);
        UnloadBitmap(&TestBitmap);
    }

    return Result;
}

static void
Init(game_state *GameState)
{
    GameState->TestTexture = LoadTexture("assets://test.png");
}

static void
DoAdvance(game_state *GameState, renderer_context *RendererContext, const input *Input)
{
    GameState->Counter += Input->DeltaTime;

    if (GameState->TestTexture)
    {
        RendererRenderTexturedQuad2(RendererContext, 0, GameState->TestTexture, 0);
    }
}

static GAME_ADVANCE(Advance)
{
    DoAdvance((game_state *) GameState, RendererContext, Input);
}

extern "C" EXPORT GAME_LOAD(GameLoad)
{
    GlobalMemory = *Module->Memory;
    GlobalFile = *Module->File;
    GlobalRenderer = *Module->Renderer;

    Game->Advance = &Advance;

    if (!Game->GameState)
    {
        game_state *GameState = (game_state *) GlobalMemory.Allocate(sizeof(*GameState));
        Init(GameState);

        Game->GameState = GameState;
    }
}
