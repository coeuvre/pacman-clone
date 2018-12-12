#include "platform_game_bridge.h"

#include <stdio.h>

#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

platform *GlobalPlatform;

struct game_state
{
    float Counter;
};

static void
InitGameState(game_state *GameState)
{
    GameState->Counter = 0.0F;
}

static void
DoFrame(game_state *GameState, game_input *Input)
{
    GameState->Counter += Input->DeltaTime;
}

extern "C"
{

EXPORT GAME_INIT(Init)
{
    GlobalPlatform = Platform;

    if (!Platform->GameState)
    {
        Platform->GameState = Platform->AllocateMemory(sizeof(game_state));
        InitGameState((game_state *) Platform->GameState);

        size_t FileSize;
        void *FileContent = Platform->ReadEntireFile("assets://test.png", &FileSize);
        if (FileContent)
        {
            uint8_t *Pixels = stbi_load_from_memory((const uint8_t *) FileContent, (int) FileSize, 0, 0, 0, 4);
            Platform->DeallocateMemory(FileContent);
            stbi_image_free(Pixels);
        }
    }
}

EXPORT GAME_UPDATE(Update)
{
    game_state *GameState = (game_state *) Platform->GameState;
    DoFrame(GameState, Input);
}

EXPORT GAME_RENDER(Render)
{
}

}
