#include "platform.h"
#include "renderer.h"

#include <stdio.h>

platform GlobalPlatform = {};

#define STBI_ONLY_PNG
#define STBI_MALLOC(Size) PlatformAllocateMemory(Size)
#define STBI_REALLOC(Pointer, NewSize) PlatformReallocateMemory(Pointer, NewSize)
#define STBI_FREE(Pointer) PlatformDeallocateMemory(Pointer)
#define STB_IMAGE_IMPLEMENTATION

#include <stb/stb_image.h>

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
};

static bitmap *
LoadBitmap(const char *URL)
{
    bitmap *Bitmap = (bitmap *) PlatformAllocateMemory(sizeof(Bitmap));
    size_t FileSize;
    void *FileContent = PlatformReadEntireFile(URL, &FileSize);
    if (FileContent)
    {
        Bitmap->Bytes = stbi_load_from_memory((const uint8_t *) FileContent, (int) FileSize,
                                              &Bitmap->Width, &Bitmap->Height, 0, 4);
        Bitmap->ChannelsPerPixel = 4;
        PlatformDeallocateMemory(FileContent);
    }
    return Bitmap;
}

static void
UnloadBitmap(bitmap **Bitmap)
{
    stbi_image_free((*Bitmap)->Bytes);
    PlatformDeallocateMemory(*Bitmap);
    *Bitmap = 0;
}

//static renderer_texture *
//LoadTexture(const char *URL)
//{
//    bitmap *TestBitmap = LoadBitmap(URL);
//    renderer_texture *Texture =
//        RendererLoadTexture(TestBitmap->Width, TestBitmap->Height, TestBitmap->ChannelsPerPixel, TestBitmap->Bytes);
//    UnloadBitmap(&TestBitmap);
//    return Texture;
//}

static void
Init(game_state *GameState)
{
//    GameState->TestTexture = LoadTexture("assets://test.png");
}

static void
Update(game_state *GameState, const input *Input)
{
    GameState->Counter += Input->DeltaTime;
}

static void
Render(game_state *GameState)
{
//    RendererRenderTexture(0, GameState->TestTexture, 0);
}

extern "C"
{

EXPORT GAME_LOAD(GameLoad)
{
    GlobalPlatform = *Platform;
}

EXPORT GAME_INIT(GameInit)
{
    game_state *GameState = (game_state *) PlatformAllocateMemory(sizeof(game_state));

    Init(GameState);

    return GameState;
}

EXPORT GAME_UPDATE(GameUpdate)
{
    Update((game_state *) GameState, Input);
}

EXPORT GAME_RENDER(GameRender)
{
}

}
