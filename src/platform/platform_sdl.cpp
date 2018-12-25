#include <SDL.h>

#include "platform/platform.h"
#include "game/game.h"

#include "core/string.cpp"

static ALLOCATE_MEMORY(SDLAllocateMemory)
{
    return malloc(Size);
}

allocate_memory_fn *AllocateMemory = &SDLAllocateMemory;

static REALLOCATE_MEMORY(SDLReallocateMemory)
{
    return realloc(Pointer, NewSize);
}

reallocate_memory_fn *ReallocateMemory = &SDLReallocateMemory;

static DEALLOCATE_MEMORY(SDLDeallocateMemory)
{
    free(Pointer);
}

deallocate_memory_fn *DeallocateMemory = &SDLDeallocateMemory;

static READ_ENTIRE_FILE(SDLReadEntireFile)
{
    const char AssetPrefix[] = "assets://";
    // TODO: Use platform_api dependent assets path
    const char *AssetDir = "assets/";

    const char DataPrefix[] = "data://";
    // TODO: Use platform_api dependent data path
    const char *DataDir = "data/";

#define BufferSize 4096
    char Buffer[BufferSize];
    if (IsStringStartWith(URL, AssetPrefix))
    {
        const char *PathInAssetDir = &URL[ArrayCount(AssetPrefix) - 1];
        ConcatString(Buffer, BufferSize, AssetDir, PathInAssetDir);
    }
    else if (IsStringStartWith(URL, DataPrefix))
    {
        const char *PathInDataDir = &URL[ArrayCount(DataPrefix) - 1];
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

                FileContent = AllocateMemory(TotalBytes);
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
                    DeallocateMemory(FileContent);
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

read_entire_file_fn *ReadEntireFile = &SDLReadEntireFile;

static render_context *RenderContext;

static render_context *
GetRenderContext()
{
    return RenderContext;
}

#include "platform/renderer_opengl.cpp"

load_texture_fn *LoadTexture = &OpenGLLoadTexture;

static input *Input;

static input *
GetInput()
{
    return Input;
}

struct sdl_game_module
{
    bool IsLoaded;
    void *Library;
    game_load_fn *Load;
    void *GameState;
    game_module Callback;
};

static void
SDLLoadGameModule(sdl_game_module *GameModule, const game_dependencies *Dependencies)
{
    if (GameModule->Library)
    {
        SDL_UnloadObject(GameModule->Library);
    }

    GameModule->Library = SDL_LoadObject("libgame.dylib");
    if (GameModule->Library)
    {
        GameModule->Load = (game_load_fn *) SDL_LoadFunction(GameModule->Library, "GameLoad");
    }

    GameModule->IsLoaded = GameModule->Library && GameModule->Load;

    if (GameModule->IsLoaded)
    {
        GameModule->Callback = GameModule->Load(Dependencies);
        if (!GameModule->GameState)
        {
            GameModule->GameState = GameModule->Callback.InitGame();
        }
    }
    else
    {
        // SDLLoadGameModule failed
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s\n", SDL_GetError());
    }
}

static void
SDLRunMainLoop(SDL_Window *Window)
{
    RenderContext = (render_context *) AllocateMemory(sizeof(*RenderContext));
    *RenderContext = {};

    OpenGLInit();

    Input = (input *) AllocateMemory(sizeof(*Input));
    Input->DeltaTime = 0.016667F;

    game_dependencies Dependencies = {
        .AllocateMemory = AllocateMemory,
        .ReallocateMemory = ReallocateMemory,
        .DeallocateMemory = DeallocateMemory,
        .ReadEntireFile = ReadEntireFile,
        .GetRenderContext = &GetRenderContext,
        .LoadTexture = LoadTexture,
        .GetInput = &GetInput,
    };

    sdl_game_module GameModule = {};
    SDLLoadGameModule(&GameModule, &Dependencies);

    Uint64 Frequency = SDL_GetPerformanceFrequency();
    Uint64 CounterPerFrame = (Uint64) (Input->DeltaTime * Frequency);
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
        SDLLoadGameModule(&GameModule, &Dependencies);
        if (GameModule.IsLoaded)
        {
            int WindowWidth, WindowHeight;
            SDL_GetWindowSize(Window, &WindowWidth, &WindowHeight);
            OpenGLBeginFrame((uint32_t) WindowWidth, (uint32_t) WindowHeight);
            GameModule.Callback.UpdateGame(GameModule.GameState);
            OpenGLEndFrame();

            OpenGLRender();
            SDL_GL_SwapWindow(Window);
        }

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

    if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
    {
//        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
//        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
//        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

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
