#include "game.h"

#include <SDL2/SDL.h>

int
main(int argc, char *argv[])
{
    (void)argc; (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO) == 0)
    {
        SDL_Window *Window = SDL_CreateWindow(
            "PacMan",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            1280,
            720,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL
        );
        if (Window)
        {
            b32 IsRunning = true;
            while (IsRunning)
            {
                SDL_Event Event;
                while (SDL_PollEvent(&Event))
                {
                    switch (Event.type)
                    {
                        case SDL_QUIT:
                        {
                            IsRunning = false;
                        } break;

                        default:
                        {
                        }
                    }
                }
            }
        }
        else
        {
            // TODO: SDL_CreateWindow failed
        }

        SDL_Quit();
    }
    else
    {
        // TODO: SDL_Init failed
    }

    return 0;
}
