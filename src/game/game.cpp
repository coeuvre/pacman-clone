#include "game.h"

#include "game/globals.cpp"

#define STBI_ONLY_PNG
#define STBI_MALLOC(Size) AllocateMemory(Size)
#define STBI_REALLOC(Pointer, NewSize) ReallocateMemory(Pointer, NewSize)
#define STBI_FREE(Pointer) DeallocateMemory(Pointer)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "renderer/renderer.cpp"
#include "game/freetype.cpp"

struct bitmap
{
    uint32_t Width;
    uint32_t Height;
    uint32_t ChannelsPerPixel;
    int32_t Pitch;
    uint8_t *Bytes;
};

struct game_state
{
    float Counter;
    texture *TestTexture;
    ft_instance *FTInstance;
    font *Font;
};

static bitmap *
LoadBitmap(const char *URL)
{
    bitmap *Result = 0;

    size_t FileSize;
    void *FileContent = ReadEntireFile(URL, &FileSize);
    if (FileContent)
    {
        int Width, Height;
        uint8_t *Bytes = stbi_load_from_memory((const uint8_t *) FileContent, (int) FileSize,
                                              &Width, &Height, 0, 4);
        if (Bytes)
        {
            Result = (bitmap *) AllocateMemory(sizeof(*Result));
            Result->Bytes = Bytes;
            Result->Width = (uint32_t) Width;
            Result->Height = (uint32_t) Height;
            Result->ChannelsPerPixel = 4;
            Result->Pitch = Width * Result->ChannelsPerPixel;
        }

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
                             TestBitmap->ChannelsPerPixel, TestBitmap->Pitch,
                             TestBitmap->Bytes);
        UnloadBitmap(&TestBitmap);
    }

    return Result;
}

static void OnGameReload(game_state *GameState)
{
    if (GameState->FTInstance)
    {
        BindFTMemoryCallback(GameState->FTInstance->Memory);
    }
}

static game_state *
LoadGameState()
{
    game_state *GameState = (game_state *) AllocateMemory(sizeof(*GameState));
    *GameState = {};
    GameState->TestTexture = LoadTextureFromURL("assets://test.png");
    GameState->FTInstance = LoadFTInstance();
    GameState->Font = LoadFontFromURL(GameState->FTInstance, "assets://test_font.otf");
    return GameState;
}

static UPDATE_GAME(UpdateGame)
{
    input* Input = GetInput();
    GameState->Counter += Input->DeltaTime;

    {
        render_command_buffer *CommandBuffer = BeginRenderCommand();
        if (GameState->TestTexture)
        {
            rect2 DstRect = Rect2MinSize(50.0F, 10.0F + GameState->Counter * 10.0F, GameState->TestTexture->Width, GameState->TestTexture->Height);
            rect2 SrcRect = Rect2MinSize(0.0F, 0.0F, GameState->TestTexture->Width, GameState->TestTexture->Height);
            PushTexturedRectangle2(CommandBuffer, &DstRect, GameState->TestTexture, &SrcRect);
        }

        font *Font = GameState->Font;
        {
            FT_Set_Pixel_Sizes(Font->Face, 0, 64);
            FT_Load_Char(Font->Face, 'F', FT_LOAD_RENDER);
            FT_GlyphSlot Slot = Font->Face->glyph;
            texture *Texture = LoadTexture(Slot->bitmap.width, Slot->bitmap.rows, 1, Slot->bitmap.pitch, Slot->bitmap.buffer);
            rect2 DstRect = Rect2MinSize(10.0F, 10.0F, Texture->Width, Texture->Height);
            PushTexturedRectangle2(CommandBuffer, &DstRect, Texture, 0);
            UnloadTexture(Texture);
        }

        EndRenderCommand(CommandBuffer);
    }
}

extern "C" EXPORT INIT_GAME_MODULE(GameLoad)
{
    BindGlobalFunctions(Dependencies);

    if (*GameStatePtr)
    {
        OnGameReload(*GameStatePtr);
    }
    else
    {
        *GameStatePtr = LoadGameState();
    }

    game_module Result = {};
    Result.UpdateGame = &UpdateGame;
    return Result;
}
