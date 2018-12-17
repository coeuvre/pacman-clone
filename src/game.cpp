#include "game.h"

#include <stdio.h>

const platform *GlobalPlatform;

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
    void *Pixels;
};

struct game_state
{
    float Counter;
    bitmap TestBitmap;
};

static void
LoadBitmap(bitmap *Bitmap, const char *URL)
{
    size_t FileSize;
    void *FileContent = PlatformReadEntireFile(URL, &FileSize);
    if (FileContent)
    {
        Bitmap->Pixels = stbi_load_from_memory((const uint8_t *) FileContent, (int) FileSize,
                                               &Bitmap->Width, &Bitmap->Height, 0, 4);
        Bitmap->ChannelsPerPixel = 4;
        PlatformDeallocateMemory(FileContent);
    }
}

static void
Init(game_state *GameState)
{
    LoadBitmap(&GameState->TestBitmap, "assets://test.png");
}

static void
Update(game_state *GameState, const input *Input)
{
    GameState->Counter += Input->DeltaTime;
}

extern "C"
{

EXPORT GAME_LOAD(GameLoad)
{
    GlobalPlatform = Platform;
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
