#include "platform_game_bridge.h"

#include <stdio.h>

const platform *GlobalPlatform;

#define STBI_ONLY_PNG
#define STBI_MALLOC(Size) (GlobalPlatform->AllocateMemory(Size))
#define STBI_REALLOC(Pointer, NewSize) (GlobalPlatform->ReallocateMemory(Pointer, NewSize))
#define STBI_FREE(Pointer) (GlobalPlatform->DeallocateMemory(Pointer))
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
LoadBitmap(bitmap *Bitmap, const platform *Platform, const char *URL)
{
    size_t FileSize;
    void *FileContent = Platform->ReadEntireFile(URL, &FileSize);
    if (FileContent)
    {
        Bitmap->Pixels = stbi_load_from_memory((const uint8_t *) FileContent, (int) FileSize,
                                               &Bitmap->Width, &Bitmap->Height, 0, 4);
        Bitmap->ChannelsPerPixel = 4;
        Platform->DeallocateMemory(FileContent);
    }
}

static void
DoInit(game_state *GameState, const platform *Platform)
{
    LoadBitmap(&GameState->TestBitmap, Platform, "assets://test.png");
}

static void
DoUpdate(game_state *GameState, const platform *Platform, const input *Input)
{
    GameState->Counter += Input->DeltaTime;
}

extern "C"
{

EXPORT GAME_LOAD(Load)
{
    GlobalPlatform = Platform;
}

EXPORT GAME_INIT(Init)
{
    game_state *GameState = (game_state *) Platform->AllocateMemory(sizeof(game_state));

    DoInit(GameState, Platform);

    return GameState;
}

EXPORT GAME_UPDATE(Update)
{
    DoUpdate((game_state *) GameState, Platform, Input);
}

EXPORT GAME_RENDER(Render)
{
}

}
