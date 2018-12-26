#include "game/bitmap.cpp"
#include "game/freetype.cpp"

struct texture_cache_entry
{
    font *Font;
    uint32_t FontPixelSize;
    uint64_t Char;

    texture *Texture;
};

struct texture_cache
{
    uint32_t EntryCountMax;
    uint32_t EntryCount;
    texture_cache_entry *Entries;
};

static void
InitTextureCache(texture_cache *Cache)
{
    Cache->EntryCountMax = 4096;
    Cache->EntryCount = 0;
    Cache->Entries = (texture_cache_entry *) AllocateMemory(sizeof(texture_cache_entry) * Cache->EntryCountMax);
}

static void
PutFontTexture(texture_cache *Cache, texture *Texture, font *Font, uint32_t FontPixelSize, uint64_t Char)
{
    Assert(Cache->EntryCount < Cache->EntryCountMax);

    texture_cache_entry *NewEntry = &Cache->Entries[Cache->EntryCount++];
    NewEntry->Font = Font;
    NewEntry->FontPixelSize = FontPixelSize;
    NewEntry->Char = Char;
    NewEntry->Texture = Texture;
}

static texture *
GetFontTexture(texture_cache *Cache, font *Font, uint32_t FontPixelSize, uint64_t Char)
{
    texture *Result = 0;

    for (uint32_t Index = 0; Index < Cache->EntryCount; ++Index)
    {
        texture_cache_entry *Entry = &Cache->Entries[Index];
        if (Entry->Font == Font && Entry->FontPixelSize == FontPixelSize && Entry->Char == Char)
        {
            Result = Entry->Texture;
            break;
        }
    }

    return Result;
}

struct game_state
{
    texture_cache TextureCache;

    float Counter;
    texture *TestTexture;
    ft_instance *FTInstance;
    font *Font;
};

static game_state *
GameLoad()
{
    game_state *GameState = (game_state *) AllocateMemory(sizeof(*GameState));
    InitTextureCache(&GameState->TextureCache);

    GameState->Counter = 0.0F;
    GameState->TestTexture = LoadTextureFromURL("assets://test.png");
    GameState->FTInstance = LoadFTInstance();
    GameState->Font = LoadFontFromURL(GameState->FTInstance, "assets://test_font.otf");
    return GameState;
}

static void
RenderTextLine(texture_cache *TextureCache, render_command_buffer *CommandBuffer, font *Font, uint32_t FontPixelSize, vec2 Pos, const char *Text)
{
    vec2 PenPos = Pos;
    const char *At = Text;
    while (*At)
    {
        FT_ULong Char = (FT_ULong) *At++;
        FT_Load_Char(Font->Face, Char, FT_LOAD_RENDER);
        FT_GlyphSlot Slot = Font->Face->glyph;

        texture *Texture = GetFontTexture(TextureCache, Font, FontPixelSize, Char);

        if (!Texture && Slot->bitmap.buffer)
        {
            Texture = LoadTexture(Slot->bitmap.width, Slot->bitmap.rows, 1, Slot->bitmap.pitch, Slot->bitmap.buffer);
            PutFontTexture(TextureCache, Texture, Font, FontPixelSize, Char);
        }

        if (Texture)
        {
            vec2 PenOffset = Vec2(Slot->bitmap_left, -Slot->bitmap_top);
            rect2 DstRect = Rect2MinSize(PenPos + PenOffset, Vec2(Texture->Width, Texture->Height));
            PushTexturedRectangle2(CommandBuffer, &DstRect, Texture, 0);
        }

        PenPos += Vec2(Slot->advance.x >> 6, -(Slot->advance.y >> 6));
    }
}

static void
RenderProfileBlock(texture_cache *TextureCache, render_command_buffer *CommandBuffer, font *Font, profile_block *ProfileBlock, uint32_t FontPixelSize, vec2 Pos, float *OffsetY = 0, int Level = 0)
{
    if (Level < 16)
    {
        vec2 PenPos = Pos + Vec2(0, FontPixelSize);
#define BufferCountMax 1024
        char Buffer[BufferCountMax];
        snprintf(Buffer, BufferCountMax, "%.2fms %s L%d %s", ProfileBlock->FrameTime, ProfileBlock->FileName, ProfileBlock->LineNumber, ProfileBlock->FunctionName);
//        snprintf(Buffer, BufferCountMax, "%.2fms", ProfileBlock->FrameTime);
        RenderTextLine(TextureCache, CommandBuffer, Font, FontPixelSize, PenPos, Buffer);

        profile_block *Child = ProfileBlock->FirstChild;
        float ChildPosAdvanceX = FontPixelSize;
        float ChildPosAdvanceY = FontPixelSize;
        vec2 ChildPos = Pos + Vec2(ChildPosAdvanceX, ChildPosAdvanceY);
        int ChildCount = 0;
        while (Child && ChildCount < 32)
        {
            float ChildOffsetY;
            RenderProfileBlock(TextureCache, CommandBuffer, Font, Child, FontPixelSize, ChildPos, &ChildOffsetY, Level + 1);
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
            RenderProfileBlock(&GameState->TextureCache, CommandBuffer, Font, ProfileFrame->RootBlock, FontPixelSize, Vec2(0, 0));
        }

        EndRenderCommand(CommandBuffer);

        PROFILE_CLOSE_BLOCK;
    }
}
