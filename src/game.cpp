#include "game.h"

#include <stdio.h>

platform *Platform;

struct game_state
{
    uint32_t Counter;
};

static void
InitGameState(game_state *GameState)
{
    GameState->Counter = 0;
}

extern "C" EXPORT void
UpdateAndRenderGame(platform *Platform_)
{
    Platform = Platform_;
    game_state *GameState = (game_state *) Platform->GameState;
    if (!GameState)
    {
        // NOTE: Init game state here
        Platform->GameState = Platform->AllocateMemory(10000);
        GameState = (game_state *) Platform->GameState;
        InitGameState(GameState);
    }

    GameState->Counter += 1;

    printf("%d\n", GameState->Counter);
}
