#include "renderer/renderer.h"

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#endif

#define TextureID(TextureHandle) ((GLuint) (TextureHandle))
#define TextureHandle(TextureId) ((uint64_t) TextureId)

static uint8_t *
ConvertBitmap(uint32_t Width, uint32_t Height, uint32_t ChannelsPerPixel, uint8_t *Bytes)
{
    Assert(ChannelsPerPixel == 4);

    size_t BitmapSize = Width * Height * ChannelsPerPixel;
    uint8_t *Result = (uint8_t *) AllocateMemory(BitmapSize);
    size_t Stride = Width * ChannelsPerPixel;

    // Flip bitmap
    uint8_t *DstRow = Result + (Height - 1) * Stride;
    uint8_t *SrcRow = Bytes;
    while (DstRow >= Result)
    {
        uint8_t *DstPixel = DstRow;
        uint8_t *SrcPixel = SrcRow;

        for (uint32_t X = 0; X < Width; ++X)
        {
            DstPixel[0] = SrcPixel[0];
            DstPixel[1] = SrcPixel[1];
            DstPixel[2] = SrcPixel[2];
            DstPixel[3] = SrcPixel[3];

            DstPixel += ChannelsPerPixel;
            SrcPixel += ChannelsPerPixel;
        }

        DstRow -= Stride;
        SrcRow += Stride;
    }

    return Result;
}

static LOAD_TEXTURE(OpenGLLoadTexture)
{
    Assert(ChannelsPerPixel == 4);

    texture *Result = 0;

    GLuint TextureID;
    glGenTextures(1, &TextureID);
    if (TextureID)
    {
        uint8_t *Pixels = ConvertBitmap(Width, Height, ChannelsPerPixel, Bytes);

        glBindTexture(GL_TEXTURE_2D, TextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, Pixels);
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

static void
OpenGLInit()
{
}

static void
OpenGLBeginFrame(uint32_t ViewportWidth, uint32_t ViewportHeight)
{
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

    glClear(GL_COLOR_BUFFER_BIT);

    vec2 InvViewportDim = 1.0F / Vec2(RenderContext->ViewportWidth, RenderContext->ViewportHeight);

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

                vec2 DstRectDim = GetRect2Dim(Data->DstRect);
                vec2 DstRectMin = Vec2(Data->DstRect.Min.X, RenderContext->ViewportHeight - Data->DstRect.Min.Y - DstRectDim.Y);
                vec2 DstRectMax = Vec2(Data->DstRect.Max.X, RenderContext->ViewportHeight - Data->DstRect.Min.Y);
                vec2 Min = Hadamard(DstRectMin, InvViewportDim) * 2.0F - Vec2(1.0F, 1.0F);
                vec2 Max = Hadamard(DstRectMax, InvViewportDim) * 2.0F - Vec2(1.0F, 1.0F);

                vec2 SrcRectDim = GetRect2Dim(Data->SrcRect);
                vec2 SrcRectMin = Vec2(Data->SrcRect.Min.X, Data->Texture.Height - Data->SrcRect.Min.Y - SrcRectDim.Y);
                vec2 SrcRectMax = Vec2(Data->SrcRect.Max.X, Data->Texture.Height - Data->SrcRect.Min.Y);
                vec2 InvTextureDim = 1.0F / Vec2(Data->Texture.Width, Data->Texture.Height);
                vec2 TexMin = Hadamard(SrcRectMin, InvTextureDim);
                vec2 TexMax = Hadamard(SrcRectMax, InvTextureDim);

                GLuint TextureID = TextureID(Data->Texture.Handle);
                glBindTexture(GL_TEXTURE_2D, TextureID);

                glBegin(GL_TRIANGLES);
                {
                    glColor4f(1.0F, 1.0F, 1.0F, 1.0F);
                    glTexCoord2f(TexMin.X, TexMin.Y);
                    glVertex4f(Min.X, Min.Y, 0.0F, 1.0F);

                    glTexCoord2f(TexMax.X, TexMin.Y);
                    glVertex4f(Max.X, Min.Y, 0.0F, 1.0F);

                    glTexCoord2f(TexMax.X, TexMax.Y);
                    glVertex4f(Max.X, Max.Y, 0.0F, 1.0F);

                    glTexCoord2f(TexMin.X, TexMin.Y);
                    glVertex4f(Min.X, Min.Y, 0.0F, 1.0F);

                    glTexCoord2f(TexMax.X, TexMax.Y);
                    glVertex4f(Max.X, Max.Y, 0.0F, 1.0F);

                    glTexCoord2f(TexMin.X, TexMax.Y);
                    glVertex4f(Min.X, Max.Y, 0.0F, 1.0F);
                }
                glEnd();

                glBindTexture(GL_TEXTURE_2D, 0);

                break;
            }

            default: INVALID_CODE_PATH;
        }
    }
}
