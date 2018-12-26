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

static void GameUpdate(game_state *GameState)
{
    input* Input = GetInput();
    GameState->Counter += Input->DeltaTime;

    {
        render_command_buffer *CommandBuffer = BeginRenderCommand();
        if (GameState->TestTexture)
        {
            rect2 DstRect = Rect2MinSize(50.0F, 10.0F + GameState->Counter * 10.0F, GameState->TestTexture->Width, GameState->TestTexture->Height);
            rect2 SrcRect = Rect2MinSize(0.0F, 0.0F, GameState->TestTexture->Width, GameState->TestTexture->Height);
            PushTexturedRectangle2(CommandBuffer, &DstRect, GameState->TestTexture, &SrcRect);
        }

        font *Font = GameState->Font;
        if (Font)
        {
            FT_UInt FontPixelSize = 16;
            FT_Set_Pixel_Sizes(Font->Face, 0, FontPixelSize);
            vec2 PenPos = Vec2(0, FontPixelSize);
            const char *Text = "Pacman Rendering Text";
            const char *At = Text;
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
        }

        EndRenderCommand(CommandBuffer);
    }
}
