#include "platform.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "string.cpp"

static PLATFORM_ALLOCATE_MEMORY(SDLAllocateMemory)
{
    return malloc(Size);
}

static PLATFORM_REALLOCATE_MEMORY(SDLReallocateMemory)
{
    return realloc(Pointer, NewSize);
}

static PLATFORM_DEALLOCATE_MEMORY(SDLDeallocateMemory)
{
    free(Pointer);
}

static PLATFORM_READ_ENTIRE_FILE(SDLReadEntireFile)
{
    const char AssetPrefix[] = "assets://";
    // TODO: Use platform dependent assets path
    const char *AssetDir = "assets/";

    const char DataPrefix[] = "data://";
    // TODO: Use platform dependent data path
    const char *DataDir = "data/";

#define BufferSize 4096
    char Buffer[BufferSize];
    if (IsStringStartWith(URL, AssetPrefix))
    {
        const char *PathInAssetDir = &URL[ArrayLength(AssetPrefix) - 1];
        ConcatString(Buffer, BufferSize, AssetDir, PathInAssetDir);
    }
    else if (IsStringStartWith(URL, DataPrefix))
    {
        const char *PathInDataDir = &URL[ArrayLength(DataPrefix) - 1];
        ConcatString(Buffer, BufferSize, DataDir, PathInDataDir);
    }
    else
    {
        Buffer[0] = 0;
    }

    void *FileContent = 0;
    if (Buffer[0])
    {
        SDL_RWops *Handle = SDL_RWFromFile(Buffer, "rb");
        if (Handle)
        {
            Sint64 Size = SDL_RWsize(Handle);
            if (Size >= 0)
            {
                size_t TotalBytes = (size_t) Size;
                if (FileSize)
                {
                    *FileSize = TotalBytes;
                }

                FileContent = SDLAllocateMemory(TotalBytes);
                size_t TotalBytesRead = 0;
                for (;;)
                {
                    size_t NumBytesRead = SDL_RWread(Handle, FileContent, 1, TotalBytes - TotalBytesRead);
                    TotalBytesRead += NumBytesRead;
                    if (NumBytesRead == 0 || TotalBytesRead >= TotalBytes)
                    {
                        break;
                    }
                }

                if (TotalBytes != TotalBytesRead)
                {
                    // Can't read entire file
                    SDLDeallocateMemory(FileContent);
                    FileContent = 0;
                }
            }
            else
            {
                // SDL_RWsize failed
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s\n", SDL_GetError());
            }

            SDL_RWclose(Handle);
        }
        else
        {
            // SDL_RWFromFile failed
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s\n", SDL_GetError());
        }
    }
    else
    {
        // URL pattern not recognized
    }

    return FileContent;
}

struct sdl_game
{
    bool IsLoaded;
    void *Library;
    game_load_fn *Load;
    game_init_fn *Init;
    game_update_fn *Update;
    game_render_fn *Render;
};

static void
SDLLoadGame(sdl_game *Game, const platform *Platform)
{
    if (Game->Library)
    {
        SDL_UnloadObject(Game->Library);
        *Game = {};
    }

    Game->Library = SDL_LoadObject("libgame.dylib");
    if (Game->Library)
    {
        Game->Load = (game_load_fn *) SDL_LoadFunction(Game->Library, "GameLoad");
        Game->Init = (game_init_fn *) SDL_LoadFunction(Game->Library, "GameInit");
        Game->Update = (game_update_fn *) SDL_LoadFunction(Game->Library, "GameUpdate");
        Game->Render = (game_render_fn *) SDL_LoadFunction(Game->Library, "GameRender");
    }

    Game->IsLoaded = Game->Load && Game->Init && Game->Update && Game->Render;

    if (Game->IsLoaded)
    {
        Game->Load(Platform);
    }
    else
    {
        // SDLLoadGame failed
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s\n", SDL_GetError());
    }
}

static void
SDLInitPlatform(platform *Platform)
{
    Platform->AllocateMemory = &SDLAllocateMemory;
    Platform->ReallocateMemory = &SDLReallocateMemory;
    Platform->DeallocateMemory = &SDLDeallocateMemory;
    Platform->ReadEntireFile = &SDLReadEntireFile;
}

static void
SDLRunMainLoop(SDL_Window *Window)
{
    GLuint TextureHandle;
    glGenTextures(1, &TextureHandle);

    platform Platform = {};
    SDLInitPlatform(&Platform);

    sdl_game Game = {};
    SDLLoadGame(&Game, &Platform);

    void *GameState = Game.Init();

    input Input = {};
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

        // Hot reload game code every frame
        // TODO: Only reload when the game code has been changed
        SDLLoadGame(&Game, &Platform);
        if (Game.IsLoaded)
        {
            Game.Update(GameState, &Input);

            glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
            glClear(GL_COLOR_BUFFER_BIT);

            Game.Render(GameState);
        }

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
        float FrameTime = (CurrentCounter - LastCounter) * 1000.0F / Frequency;
        LastCounter = CurrentCounter;
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "FrameTime %.2fms\n", FrameTime);
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
                    SDLRunMainLoop(Window);
                }
                else
                {
                    // SDL_GL_SwapInterval failed
                    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
                }
            }
            else
            {
                // SDL_GL_CreateContext failed
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
            }
        }
        else
        {
            // SDL_CreateWindow failed
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
        }

        SDL_Quit();
    }
    else
    {
        // SDL_Init failed
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
    }

    return 0;
}