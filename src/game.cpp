#include "game.h"

#include <stdio.h>

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

extern "C" EXPORT GAME_INIT(Init)
{
    GlobalPlatform = Platform;
    if (!Platform->GameState)
    {
        GlobalPlatform->GameState = GlobalPlatform->AllocateMemory(sizeof(game_state));
        InitGameState((game_state *) GlobalPlatform->GameState);
    }
}

extern "C" EXPORT GAME_UPDATE(Update)
{
    game_state *GameState = (game_state *) GlobalPlatform->GameState;

    GameState->Counter += Input->DeltaTime;
}

extern "C" EXPORT GAME_RENDER(Render)
{
}
