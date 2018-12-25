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
    *GameState = {};
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
            FT_Set_Pixel_Sizes(Font->Face, 0, 64);
            FT_Load_Char(Font->Face, 'F', FT_LOAD_RENDER);
            FT_GlyphSlot Slot = Font->Face->glyph;
            texture *Texture = LoadTexture(Slot->bitmap.width, Slot->bitmap.rows, 1, Slot->bitmap.pitch, Slot->bitmap.buffer);
            rect2 DstRect = Rect2MinSize(10.0F, 10.0F, Texture->Width, Texture->Height);
            PushTexturedRectangle2(CommandBuffer, &DstRect, Texture, 0);
            UnloadTexture(Texture);
        }

        EndRenderCommand(CommandBuffer);
    }
}
