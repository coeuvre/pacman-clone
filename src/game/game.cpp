#include "game/bitmap.cpp"
#include "game/freetype.cpp"

struct game_state
{
    float Counter;
    texture *TestTexture;
    ft_instance *FTInstance;
    font *Font;
};

static game_state *
GameLoad()
{
    game_state *GameState = (game_state *) AllocateMemory(sizeof(*GameState));
    GameState->Counter = 0.0F;
    GameState->TestTexture = LoadTextureFromURL("assets://test.png");
    GameState->FTInstance = LoadFTInstance();
    GameState->Font = LoadFontFromURL(GameState->FTInstance, "assets://test_font.otf");
    return GameState;
}

static void
RenderProfileBlock(render_command_buffer *CommandBuffer, font *Font, profile_block *ProfileBlock, float FontPixelSize, vec2 Pos, float *OffsetY = 0, int Level = 0)
{
    if (Level < 16)
    {
        vec2 PenPos = Pos + Vec2(0, FontPixelSize);
#define BufferCountMax 1024
        char Buffer[BufferCountMax];
//        snprintf(Buffer, BufferCountMax, "%.2fms %s L%d %s", ProfileBlock->FrameTime, ProfileBlock->FileName, ProfileBlock->LineNumber, ProfileBlock->FunctionName);
        snprintf(Buffer, BufferCountMax, "%.2fms", ProfileBlock->FrameTime);
        const char *At = Buffer;
        while (*At)
        {
            FT_ULong Char = (FT_ULong) *At++;
            FT_Load_Char(Font->Face, Char, FT_LOAD_RENDER);
            FT_GlyphSlot Slot = Font->Face->glyph;
            if (Slot->bitmap.buffer)
            {
                texture *Texture = LoadTexture(Slot->bitmap.width, Slot->bitmap.rows, 1, Slot->bitmap.pitch, Slot->bitmap.buffer);
                vec2 PenOffset = Vec2(Slot->bitmap_left, -Slot->bitmap_top);
                rect2 DstRect = Rect2MinSize(PenPos + PenOffset, Vec2(Texture->Width, Texture->Height));
                PushTexturedRectangle2(CommandBuffer, &DstRect, Texture, 0);
                UnloadTexture(Texture);
            }
            PenPos += Vec2(Slot->advance.x >> 6, -(Slot->advance.y >> 6));
        }

        profile_block *Child = ProfileBlock->FirstChild;
        float ChildPosAdvanceX = FontPixelSize;
        float ChildPosAdvanceY = FontPixelSize;
        vec2 ChildPos = Pos + Vec2(ChildPosAdvanceX, ChildPosAdvanceY);
        int ChildCount = 0;
        while (Child && ChildCount < 32)
        {
            float ChildOffsetY;
            RenderProfileBlock(CommandBuffer, Font, Child, FontPixelSize, ChildPos, &ChildOffsetY, Level + 1);
            Child = Child->Next;
            ChildPos.Y = ChildPosAdvanceY + ChildOffsetY;
            ++ChildCount;
        }
        if (OffsetY)
        {
            *OffsetY = (ChildPos.Y - FontPixelSize);
        }
    }
}

static void
GameUpdate(game_state *GameState)
{
    input* Input = GetInput();
    GameState->Counter += Input->DeltaTime;

    {
        PROFILE_OPEN_BLOCK;
        render_command_buffer *CommandBuffer = BeginRenderCommand();
        if (GameState->TestTexture)
        {
            rect2 DstRect = Rect2MinSize(100.0F, 10.0F + GameState->Counter * 10.0F, GameState->TestTexture->Width, GameState->TestTexture->Height);
            rect2 SrcRect = Rect2MinSize(0.0F, 0.0F, GameState->TestTexture->Width, GameState->TestTexture->Height);
            PushTexturedRectangle2(CommandBuffer, &DstRect, GameState->TestTexture, &SrcRect);
        }
        PROFILE_CLOSE_BLOCK;

        PROFILE_OPEN_BLOCK;
        font *Font = GameState->Font;
        profile_frame *ProfileFrame = GetLastProfileFrame();
        if (Font && ProfileFrame)
        {
            FT_UInt FontPixelSize = 16;
            FT_Set_Pixel_Sizes(Font->Face, 0, FontPixelSize);
            RenderProfileBlock(CommandBuffer, Font, ProfileFrame->RootBlock, FontPixelSize, Vec2(0, 0));
        }

        EndRenderCommand(CommandBuffer);

        PROFILE_CLOSE_BLOCK;
    }
}
