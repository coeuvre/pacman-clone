#include "platform_game_bridge.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

static void *
SDLAllocate(size_t Size)
{
    return malloc(Size);
}

static void
SDLDeallocate(void *Ptr)
{
    free(Ptr);
}

struct sdl_game
{
    bool IsLoaded;
    void *Library;
    update_and_render_game_fn *UpdateAndRender;
};

static void
SDLLoadGame(sdl_game *Game)
{
    Game->Library = SDL_LoadObject("libgame.dylib");
    if (Game->Library)
    {
        Game->UpdateAndRender = (update_and_render_game_fn *) SDL_LoadFunction(Game->Library, "UpdateAndRenderGame");
        if (!Game->UpdateAndRender)
        {
            // TODO: Failed to load UpdateAndRenderGame
            printf("%s\n", SDL_GetError());
        }
    }
    else
    {
        // TODO: Failed to load game library
        printf("%s\n", SDL_GetError());
    }

    Game->IsLoaded = Game->Library && Game->UpdateAndRender;
}

static void
SDLRunMainLoop(SDL_Window *Window, SDL_GLContext GLContext, int WindowWidth, int WindowHeight)
{
    GLuint TextureHandle;
    glGenTextures(1, &TextureHandle);

    platform Platform = {};
    Platform.AllocateMemory = &SDLAllocate;
    Platform.DeallocateMemory = &SDLDeallocate;

    sdl_game Game = {};
    SDLLoadGame(&Game);

    if (Game.IsLoaded)
    {
        bool IsRunning = true;
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
                        break;
                    }

                    default:
                    {
                    }
                }
            }

            Game.UpdateAndRender(&Platform);

            // glBindTexture(GL_TEXTURE_2D, TextureHandle);
            // //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, Pixels);
            // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

            // glViewport(0, 0, WindowWidth, WindowHeight);

            // glEnable(GL_TEXTURE_2D);

            glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // glMatrixMode(GL_TEXTURE);
            // glLoadIdentity();

            // glMatrixMode(GL_MODELVIEW);
            // glLoadIdentity();
            // glMatrixMode(GL_PROJECTION);
            // glLoadIdentity();

            // f32 P = 0.9f;

            // glBegin(GL_TRIANGLES);
            // glTexCoord2f(0.0f, 0.0f);
            // glVertex2f(-P, -P);

            // glTexCoord2f(1.0f, 0.0f);
            // glVertex2f(P, -P);

            // glTexCoord2f(1.0f, 1.0f);
            // glVertex2f(P, P);

            // glTexCoord2f(0.0f, 0.0f);
            // glVertex2f(-P, -P);

            // glTexCoord2f(1.0f, 1.0f);
            // glVertex2f(P, P);

            // glTexCoord2f(0.0f, 1.0f);
            // glVertex2f(-P, P);
            // glEnd();

            SDL_GL_SwapWindow(Window);
        }
    }
    else
    {
        // TODO: SDLLoadGame failed
    }
}

int
main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    int WindowWidth = 1280;
    int WindowHeight = 720;

    if (SDL_Init(SDL_INIT_VIDEO) == 0)
    {
        SDL_Window *Window = SDL_CreateWindow(
            "PacMan",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            WindowWidth,
            WindowHeight,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
        if (Window)
        {
            SDL_GLContext GLContext = SDL_GL_CreateContext(Window);

            if (GLContext)
            {
                if (SDL_GL_MakeCurrent(Window, GLContext) == 0)
                {
                    if (SDL_GL_SetSwapInterval(1) == 0)
                    {
                        SDLRunMainLoop(Window, GLContext, WindowWidth, WindowHeight);
                    }
                    else
                    {
                        // TODO: SDL_GL_SwapWindow failed
                    }
                }
                else
                {
                    // TODO: SDL_GL_MakeCurrent failed
                }
            }
            else
            {
                // TODO: SDL_GL_CreateContext failed
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
