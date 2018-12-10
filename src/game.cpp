#include "platform_game_bridge.h"

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
        GlobalPlatform->GameState = GlobalPlatform->AllocateMemory(sizeof(game_state));
        InitGameState((game_state *) GlobalPlatform->GameState);
    }
}

EXPORT GAME_UPDATE(Update)
{
    game_state *GameState = (game_state *) GlobalPlatform->GameState;
    DoFrame(GameState, Input);
}

EXPORT GAME_RENDER(Render)
{
}

}
