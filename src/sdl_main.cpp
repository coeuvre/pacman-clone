#include "game.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

internal void
RunMainLoop(SDL_Window *Window, SDL_GLContext GLContext, int WindowWidth, int WindowHeight)
{
    (void)GLContext;

    GLuint TextureHandle;
    glGenTextures(1, &TextureHandle);

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

        glBindTexture(GL_TEXTURE_2D, TextureHandle);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, Pixels);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        glViewport(0, 0, WindowWidth, WindowHeight);

        glEnable(GL_TEXTURE_2D);

        glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_TEXTURE);
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        f32 P = 0.9f;

        glBegin(GL_TRIANGLES);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-P, -P);

        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(P, -P);

        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(P, P);

        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-P, -P);

        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(P, P);

        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(-P, P);
        glEnd();

        SDL_GL_SwapWindow(Window);
    }
}

int
main(int argc, char *argv[])
{
    (void)argc; (void)argv;

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
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
        );
        if (Window)
        {
            SDL_GLContext GLContext = SDL_GL_CreateContext(Window);

            if (GLContext)
            {
                if (SDL_GL_MakeCurrent(Window, GLContext) == 0)
                {
                    if (SDL_GL_SetSwapInterval(1) == 0)
                    {
                      RunMainLoop(Window, GLContext, WindowWidth, WindowHeight);
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
