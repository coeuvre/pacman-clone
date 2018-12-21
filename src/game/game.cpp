#include "game.h"

#include "game/globals.cpp"

#define STBI_ONLY_PNG
#define STBI_MALLOC(Size) AllocateMemory(Size)
#define STBI_REALLOC(Pointer, NewSize) ReallocateMemory(Pointer, NewSize)
#define STBI_FREE(Pointer) DeallocateMemory(Pointer)
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
    texture *TestTexture;
};

static bitmap *
LoadBitmap(const char *URL)
{
    bitmap *Result = 0;

    size_t FileSize;
    void *FileContent = ReadEntireFile(URL, &FileSize);
    if (FileContent)
    {
        Result = (bitmap *) AllocateMemory(sizeof(*Result));
        Result->Bytes = stbi_load_from_memory((const uint8_t *) FileContent, (int) FileSize,
                                              &Result->Width, &Result->Height, 0, 4);
        Result->ChannelsPerPixel = 4;

        DeallocateMemory(FileContent);
    }
    return Result;
}

static void
UnloadBitmap(bitmap **Bitmap)
{
    stbi_image_free((*Bitmap)->Bytes);
    DeallocateMemory(*Bitmap);
    *Bitmap = 0;
}

static texture *
LoadTextureFromURL(const char *URL)
{
    texture *Result = 0;

    bitmap *TestBitmap = LoadBitmap(URL);
    if (TestBitmap)
    {
        Result = LoadTexture(TestBitmap->Width, TestBitmap->Height,
                             TestBitmap->ChannelsPerPixel,
                             TestBitmap->Bytes);
        UnloadBitmap(&TestBitmap);
    }

    return Result;
}

static void
DoUpdateGameState(game_state *GameState)
{
    render_command_buffer *CommandBuffer = BeginRenderCommand();

    input* Input = GetInput();
    GameState->Counter += Input->DeltaTime;

    if (GameState->TestTexture)
    {
        rect2 DstRect = Rect2MinSize(50.0F, 10.0F, 100.0F, 100.0F);
        rect2 SrcRect = Rect2MinSize(0.0F, 10.0F * GameState->Counter, 50.0F, 50.0F);
        PushTexturedRectangle2(CommandBuffer, &DstRect, GameState->TestTexture, &SrcRect);
    }

    EndRenderCommand(CommandBuffer);
}

static game_state *
DoInitGameState()
{
    game_state *GameState = (game_state *) AllocateMemory(sizeof(*GameState));
    *GameState = {};
    GameState->TestTexture = LoadTextureFromURL("assets://test.png");

    return GameState;
}

static INIT_GAME_STATE(InitGameState)
{
    return DoInitGameState();
}

static UPDATE_GAME_STATE(UpdateGameState)
{
    DoUpdateGameState((game_state *) GameState);
}

extern "C" EXPORT INIT_GAME_MODULE(GameLoad)
{
    InitGlobals(Dependencies);

    game_module Result = {};
    Result.InitGameState = &InitGameState;
    Result.UpdateGameState = &UpdateGameState;
    return Result;
}
