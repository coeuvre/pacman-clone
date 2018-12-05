#include "game.h"

#include <stdio.h>

platform *Platform;

struct game_state {
    u32 Counter;
};

static void
InitGameState(game_state *GameState)
{
    GameState->Counter = 0;
}

extern "C" void
UpdateAndRenderGame(platform *Platform_)
{
    Platform = Platform_;
    game_state *GameState = (game_state *)Platform->GameState;
    if (!GameState)
    {
        // NOTE: Init game state here
        Platform->GameState = Platform->Allocate(10000);
        GameState = (game_state *)Platform->GameState;
        InitGameState(GameState);
    }

    GameState->Counter += 1;

    printf("%d\n", GameState->Counter);
}
