struct RenderTexturedRect2VertexAttrib {
    GLfloat Position[2];
    GLfloat TexCoord[2];
    GLfloat Color[4];
};

const char RenderTexturedRect2VertexShaderSource[] = R"(
#version 330 core

layout (location = 0) in vec2 AttribPosition;
layout (location = 1) in vec2 AttribTexCoord;
layout (location = 2) in vec4 AttribColor;

out vec2 VertexTexCoord;
out vec4 VertexColor;

void main()
{
    gl_Position = vec4(AttribPosition, 0, 1);
    VertexTexCoord = AttribTexCoord;
    VertexColor = AttribColor;
}
)";

const char RenderTexturedRect2FragmentShaderSource[] = R"(
#version 330 core

uniform sampler2D UniformTexture0;

in vec2 VertexTexCoord;
in vec4 VertexColor;

out vec4 FragColor;

void main()
{
    vec4 Texel = texture(UniformTexture0, VertexTexCoord);
    FragColor = Texel * VertexColor;
}
)";

struct opengl_render_textured_rect2_program
{
    GLuint ID;
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
};

static opengl_render_textured_rect2_program *
OpenGLLoadRenderTexturedRect2Program()
{
    opengl_render_textured_rect2_program *Result = 0;

    GLuint ID = OpenGLCompileProgram(RenderTexturedRect2VertexShaderSource,
                                     RenderTexturedRect2FragmentShaderSource);
    if (ID)
    {
        Result = (opengl_render_textured_rect2_program *) AllocateMemory(sizeof(*Result));
        Result->ID = ID;

        glGenVertexArrays(1, &Result->VAO);
        glGenBuffers(1, &Result->VBO);
        glGenBuffers(1, &Result->EBO);

        glBindVertexArray(Result->VAO);
        glBindBuffer(GL_ARRAY_BUFFER, Result->VBO);
        glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STREAM_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Result->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, NULL, GL_STREAM_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                              sizeof(RenderTexturedRect2VertexAttrib),
                              (void *) offsetof(RenderTexturedRect2VertexAttrib, Position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                              sizeof(RenderTexturedRect2VertexAttrib),
                              (void *)offsetof(RenderTexturedRect2VertexAttrib, TexCoord));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE,
                              sizeof(RenderTexturedRect2VertexAttrib),
                              (void *)offsetof(RenderTexturedRect2VertexAttrib, Color));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
//
//        glUseProgram(Result->Program);
//        glUniform1i(glGetUniformLocation(drawTextureProgram->program, "texture0"), 0);
//        drawTextureProgram->MVPLocation =
//            glGetUniformLocation(drawTextureProgram->program, "MVP");
    }

    return Result;
}

static void
RenderTexturedRect2(opengl_render_textured_rect2_program *Program, render_context *RenderContext, textured_rect2 *Data)
{
    texture *Texture = Data->Texture;

    vec2 InvViewportDim = 1.0F / Vec2(RenderContext->ViewportWidth, RenderContext->ViewportHeight);

    vec2 DstRectDim = GetRect2Dim(Data->DstRect);
    vec2 DstRectMin = Vec2(Data->DstRect.Min.X, RenderContext->ViewportHeight - Data->DstRect.Min.Y - DstRectDim.Y);
    vec2 DstRectMax = Vec2(Data->DstRect.Max.X, RenderContext->ViewportHeight - Data->DstRect.Min.Y);
    vec2 Min = Hadamard(DstRectMin, InvViewportDim) * 2.0F - Vec2(1.0F, 1.0F);
    vec2 Max = Hadamard(DstRectMax, InvViewportDim) * 2.0F - Vec2(1.0F, 1.0F);

    vec2 SrcRectDim = GetRect2Dim(Data->SrcRect);
    vec2 SrcRectMin = Vec2(Data->SrcRect.Min.X, Texture->Height - Data->SrcRect.Min.Y - SrcRectDim.Y);
    vec2 SrcRectMax = Vec2(Data->SrcRect.Max.X, Texture->Height - Data->SrcRect.Min.Y);
    vec2 InvTextureDim = 1.0F / Vec2(Texture->Width, Texture->Height);
    vec2 TexMin = Hadamard(SrcRectMin, InvTextureDim);
    vec2 TexMax = Hadamard(SrcRectMax, InvTextureDim);

    RenderTexturedRect2VertexAttrib Vertices[] = {
        // Bottom Left
        {
            .Position = { Min.X, Min.Y },
            .TexCoord = { TexMin.X, TexMin.Y },
            .Color = { 1.0F, 1.0F, 1.0F, 1.0F },
        },
        // Bottom Right
        {
            .Position = { Max.X, Min.Y },
            .TexCoord = { TexMax.X, TexMin.Y },
            .Color = { 1.0F, 1.0F, 1.0F, 1.0F },
        },
        // Top Right
        {
            .Position = { Max.X, Max.Y },
            .TexCoord = { TexMax.X, TexMax.Y },
            .Color = { 1.0F, 1.0F, 1.0F, 1.0F },
        },
        // Top Left
        {
            .Position = { Min.X, Max.Y },
            .TexCoord = { TexMin.X, TexMax.Y },
            .Color = { 1.0F, 1.0F, 1.0F, 1.0F },
        },
    };

    GLuint Indices[] = {
        0, 1, 2,
        0, 2, 3,
    };

    glBindBuffer(GL_ARRAY_BUFFER, Program->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STREAM_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Program->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STREAM_DRAW);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, (GLuint) Texture->Handle);

    glUseProgram(Program->ID);

    glBindVertexArray(Program->VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    UnloadTexture(Texture);
}
