#define STBI_ONLY_PNG
#define STBI_MALLOC(Size) AllocateMemory(Size)
#define STBI_REALLOC(Pointer, NewSize) ReallocateMemory(Pointer, NewSize)
#define STBI_FREE(Pointer) DeallocateMemory(Pointer)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

struct bitmap
{
    uint32_t Width;
    uint32_t Height;
    uint32_t ChannelsPerPixel;
    int32_t Pitch;
    uint8_t *Bytes;
};

static bitmap *
LoadBitmap(const char *URL)
{
    bitmap *Result = 0;

    size_t FileSize;
    uint8_t *FileContent = ReadEntireFile(URL, &FileSize);
    if (FileContent)
    {
        int Width, Height;
        uint8_t *Bytes = stbi_load_from_memory(FileContent, (int) FileSize,
                                               &Width, &Height, 0, 4);
        if (Bytes)
        {
            Result = (bitmap *) AllocateMemory(sizeof(*Result));
            Result->Bytes = Bytes;
            Result->Width = (uint32_t) Width;
            Result->Height = (uint32_t) Height;
            Result->ChannelsPerPixel = 4;
            Result->Pitch = Width * Result->ChannelsPerPixel;
        }

        DeallocateMemory(FileContent);
    }
    return Result;
}

static void
UnloadBitmap(bitmap **Bitmap)
{
    stbi_image_free((*Bitmap)->Bytes);
    DeallocateMemory(*Bitmap);
    *Bitmap = 0;
}

static texture *
LoadTextureFromURL(const char *URL)
{
    texture *Result = 0;

    bitmap *TestBitmap = LoadBitmap(URL);
    if (TestBitmap)
    {
        Result = LoadTexture(TestBitmap->Width, TestBitmap->Height,
                             TestBitmap->ChannelsPerPixel, TestBitmap->Pitch,
                             TestBitmap->Bytes);
        UnloadBitmap(&TestBitmap);
    }

    return Result;
}

