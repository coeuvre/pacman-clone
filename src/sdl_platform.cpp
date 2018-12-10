#include "platform_game_bridge.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

static void *
SDLAllocateMemory(size_t Size)
{
    return malloc(Size);
}

static void
SDLDeallocateMemory(void *Ptr)
{
    free(Ptr);
}

struct sdl_game
{
    bool IsLoaded;
    void *Library;
    game_init_fn *Init;
    game_update_fn *Update;
    game_render_fn *Render;
};

static void
SDLLoadGame(sdl_game *Game)
{
    Game->Library = SDL_LoadObject("libgame.dylib");
    if (Game->Library)
    {
        Game->Init = (game_init_fn *) SDL_LoadFunction(Game->Library, "Init");
        Game->Update = (game_update_fn *) SDL_LoadFunction(Game->Library, "Update");
        Game->Render = (game_render_fn *) SDL_LoadFunction(Game->Library, "Render");
    }

    Game->IsLoaded = Game->Library && Game->Init && Game->Update && Game->Render;
}

static void
SDLInitPlatform(platform *Platform)
{
    Platform->AllocateMemory = &SDLAllocateMemory;
    Platform->DeallocateMemory = &SDLDeallocateMemory;
}

static void
SDLRunMainLoop(SDL_Window *Window)
{
    GLuint TextureHandle;
    glGenTextures(1, &TextureHandle);

    sdl_game Game = {};
    SDLLoadGame(&Game);

    if (Game.IsLoaded)
    {
        platform Platform = {};
        SDLInitPlatform(&Platform);
        Game.Init(&Platform);

        game_input Input = {};
        Input.DeltaTime = 0.016667F;

        Uint64 Frequency = SDL_GetPerformanceFrequency();
        Uint64 CounterPerFrame = (Uint64) (Input.DeltaTime * Frequency);
        Uint64 CurrentCounter = SDL_GetPerformanceCounter();
        Uint64 LastCounter = CurrentCounter;

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

            Game.Update(&Input);

            // glBindTexture(GL_TEXTURE_2D, TextureHandle);
            // //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, Pixels);
            // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

            // glViewport(0, 0, WindowWidth, WindowHeight);

            // glEnable(GL_TEXTURE_2D);

            glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
            glClear(GL_COLOR_BUFFER_BIT);

            Game.Render();
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

            CurrentCounter = SDL_GetPerformanceCounter();
            Uint64 FrameCostCounter = CurrentCounter - LastCounter;
            if (CounterPerFrame > FrameCostCounter)
            {
                Uint32 SleepMS = (Uint32) ((CounterPerFrame - FrameCostCounter) * 1000 / Frequency);
                if (SleepMS > 0)
                {
                    SDL_Delay(SleepMS);
                }
            }

            CurrentCounter = SDL_GetPerformanceCounter();
            Uint32 FrameTime = (Uint32) ((CurrentCounter - LastCounter) * 1000 / Frequency);
            LastCounter = CurrentCounter;
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "FrameTime %ums\n", FrameTime);
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

    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

    if (SDL_Init(SDL_INIT_VIDEO) == 0)
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        SDL_Window *Window = SDL_CreateWindow(
            "PacMan",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            WindowWidth,
            WindowHeight,
            SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
        if (Window)
        {
            SDL_GLContext GLContext = SDL_GL_CreateContext(Window);

            if (GLContext)
            {
                if (SDL_GL_SetSwapInterval(1) == 0)
                {
                    printf("%s\n", glGetString(GL_VERSION));

                    SDLRunMainLoop(Window);
                }
                else
                {
                    // NOTE: SDL_GL_SwapInterval failed
                    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
                }
            }
            else
            {
                // NOTE: SDL_GL_CreateContext failed
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
            }
        }
        else
        {
            // NOTE: SDL_CreateWindow failed
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
        }

        SDL_Quit();
    }
    else
    {
        // NOTE: SDL_Init failed
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
    }

    return 0;
}
