#include "platform_game_bridge.h"

#include <stdio.h>

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
    if (!Platform->GameState)
    {
        Platform->GameState = Platform->AllocateMemory(sizeof(game_state));
        InitGameState((game_state *) Platform->GameState);

        Platform->ReadEntireFile("assets://test.png");
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
