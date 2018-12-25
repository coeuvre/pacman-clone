#include "renderer/renderer.h"

#include <glad/glad.h>

#define TextureID(TextureHandle) ((GLuint) (TextureHandle))
#define TextureHandle(TextureId) ((uint64_t) TextureId)

static uint8_t *
OpenGLConvertBitmap(uint32_t Width, uint32_t Height, uint32_t ChannelsPerPixel, int32_t Pitch, uint8_t *Bytes)
{
    int32_t DstPitch = Width * 4;
    size_t BitmapSize = DstPitch * Height;
    uint8_t *Result = (uint8_t *) AllocateMemory(BitmapSize);

    float Gamma = 2.2F;
    float InvGamma = 1 / Gamma;

    // Flip bitmap
    uint8_t *DstRow = Result + (Height - 1) * DstPitch;
    uint8_t *SrcRow = Bytes;
    while (DstRow >= Result)
    {
        uint8_t *DstPixel = DstRow;
        uint8_t *SrcPixel = SrcRow;

        for (uint32_t X = 0; X < Width; ++X)
        {
            vec4 Color = {};

            switch (ChannelsPerPixel)
            {
                case 4:
                {
                    Color = Vec4(SrcPixel[0], SrcPixel[1], SrcPixel[2], SrcPixel[3]);
                    Color = Color / 255.0F;

                    // sRGB to lRGB
                    Color.R = powf(Color.R, Gamma);
                    Color.G = powf(Color.G, Gamma);
                    Color.B = powf(Color.B, Gamma);

                    // Premultiply alpha
                    Color.RGB = Color.RGB * Color.A;
                    break;
                }

                case 1:
                {
                    scalar A = SrcPixel[0] / 255.0F;
                    Color = Vec4(A, A, A, A);
                    break;
                }

                default: INVALID_CODE_PATH;
            }

            // lRGB to sRGB
            Color.R = powf(Color.R, InvGamma);
            Color.G = powf(Color.G, InvGamma);
            Color.B = powf(Color.B, InvGamma);

            Color = Color * 255.0F;

            DstPixel[0] = (uint8_t) Round(Color.R);
            DstPixel[1] = (uint8_t) Round(Color.G);
            DstPixel[2] = (uint8_t) Round(Color.B);
            DstPixel[3] = (uint8_t) Round(Color.A);

            DstPixel += 4;
            SrcPixel += ChannelsPerPixel;
        }

        DstRow -= DstPitch;
        SrcRow += Pitch;
    }

    return Result;
}

static LOAD_TEXTURE(OpenGLLoadTexture)
{
    Assert(Bytes);

    texture *Result = 0;

    GLuint TextureID;
    glGenTextures(1, &TextureID);
    if (TextureID)
    {
        uint8_t *Pixels = OpenGLConvertBitmap(Width, Height, ChannelsPerPixel, Pitch, Bytes);

        glBindTexture(GL_TEXTURE_2D, TextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);

        Result = (texture *) AllocateMemory(sizeof(*Result));
        Result->Width = Width;
        Result->Height = Height;
        Result->Handle = TextureHandle(TextureID);

        DeallocateMemory(Pixels);
    }

    return Result;
}

typedef void* load_opengl_proc_fn(const char *name);

static GLuint
OpenGLCompileShader(GLenum Type, const char *Source)
{
    GLuint Shader = glCreateShader(Type);
    glShaderSource(Shader, 1, &Source, 0);

    glCompileShader(Shader);

    GLint success;
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
        char Buffer[512];
        glGetShaderInfoLog(Shader, sizeof(Buffer), 0, Buffer);
        printf("Failed to compile shader: %s\n", Buffer);
        glDeleteShader(Shader);
        Shader = 0;
    }

    return Shader;
}

static GLuint OpenGLCompileProgram(const char *VertexShaderSource, const char *FragmentShaderSource)
{
    GLuint Program = 0;

    GLuint VertexShader = OpenGLCompileShader(GL_VERTEX_SHADER, VertexShaderSource);
    if (VertexShader) {
        GLuint FragmentShader = OpenGLCompileShader(GL_FRAGMENT_SHADER, FragmentShaderSource);
        if (FragmentShader) {
            Program = glCreateProgram();
            glAttachShader(Program, VertexShader);
            glAttachShader(Program, FragmentShader);
            glLinkProgram(Program);

            GLint success;
            glGetProgramiv(Program, GL_LINK_STATUS, &success);
            if (success != GL_TRUE) {
                char Buffer[512];
                glGetProgramInfoLog(Program, sizeof(Buffer), 0, Buffer);
                printf("Failed to link program: %s\n", Buffer);
                glDeleteShader(VertexShader);
                glDeleteShader(FragmentShader);
                Program = 0;
            }
        }
        else
        {
            glDeleteShader(VertexShader);
        }
    }

    return Program;
}

#include "renderer/renderer_opengl_render_textured_rect2.cpp"

struct opengl_render_context
{
    opengl_render_textured_rect2_program *RenderTexturedRect2Program;
};

opengl_render_context GlobalOpenGLRenderContext;

static int
OpenGLInit(load_opengl_proc_fn *LoadOpenGLProcFn)
{
    int Result;

    gladLoadGLLoader(LoadOpenGLProcFn);

    GlobalOpenGLRenderContext.RenderTexturedRect2Program = OpenGLLoadRenderTexturedRect2Program();

    Result = GlobalOpenGLRenderContext.RenderTexturedRect2Program != 0;

    return Result;
}

static void
OpenGLBeginFrame(uint32_t ViewportWidth, uint32_t ViewportHeight)
{
    render_context *RenderContext = GetRenderContext();
    RenderContext->ViewportWidth = ViewportWidth;
    RenderContext->ViewportHeight = ViewportHeight;
}

static void
OpenGLEndFrame()
{

}

static void
OpenGLRender()
{
    render_context *RenderContext = GetRenderContext();
    render_command_buffer *CommandBuffer = &RenderContext->CommandBuffer;

    glClearColor(0.0F, 0.0F, 0.0F, 0.0F);
    glViewport(0, 0, RenderContext->ViewportWidth, RenderContext->ViewportHeight);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_FRAMEBUFFER_SRGB);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_MULTISAMPLE);

    glClear(GL_COLOR_BUFFER_BIT);

    for (uint8_t *At = CommandBuffer->Base; At < CommandBuffer->At; NOOP)
    {
        render_command_header *Header = (render_command_header *) At;
        At += sizeof(*Header);

        switch (Header->Type)
        {
            case RenderCommandDataType_textured_rect2:
            {
                textured_rect2 *Data = (textured_rect2 *) At;
                At += sizeof(*Data);
                RenderTexturedRect2(GlobalOpenGLRenderContext.RenderTexturedRect2Program, RenderContext, Data);
                break;
            }

            default: INVALID_CODE_PATH;
        }
    }
}
