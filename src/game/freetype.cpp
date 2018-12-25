#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MODULE_H

static void *
FTAllocate(FT_Memory Memory, long Size)
{
    (void) Memory;
    void *Result = AllocateMemory((size_t) Size);
    return Result;
}

static void *
FTReallocate(FT_Memory Memory, long CurSize, long NewSize, void *Block)
{
    (void) Memory; (void) CurSize;
    void * Result = ReallocateMemory(Block, (size_t) NewSize);
    return Result;
}

static void
FTFree(FT_Memory Memory, void *Block)
{
    (void) Memory;
    DeallocateMemory(Block);
}

struct ft_instance
{
    FT_Memory Memory;
    FT_Library Library;
};

static void
BindFTMemoryCallback(FT_Memory Memory)
{
    Memory->user = 0;
    Memory->alloc = &FTAllocate;
    Memory->realloc = &FTReallocate;
    Memory->free = &FTFree;
}

static ft_instance *
LoadFTInstance()
{
    ft_instance *Result = 0;

    FT_MemoryRec_ *Memory = (FT_MemoryRec_ *) AllocateMemory(sizeof(*Memory));
    BindFTMemoryCallback(Memory);

    FT_Library Library;
    if (FT_New_Library(Memory, &Library) == 0)
    {
        FT_Add_Default_Modules(Library);

        Result = (ft_instance *) AllocateMemory(sizeof(*Result));
        Result->Library = Library;
        Result->Memory = Memory;
    }
    else
    {
        // FT_New_Library failed
        DeallocateMemory(Memory);
    }

    return Result;
}

static void
UnloadFTInstance(ft_instance **InstancePtr)
{
    ft_instance *Instance = *InstancePtr;
    FT_Done_Library(Instance->Library);
    DeallocateMemory(Instance->Memory);
    DeallocateMemory(Instance);
    *InstancePtr = 0;
}

struct font
{
    uint8_t *FileContent;
    FT_Face Face;
};

static font *
LoadFontFromURL(ft_instance *Instance, const char *URL)
{
    font *Result = 0;

    size_t FileSize;
    uint8_t *FileContent = ReadEntireFile(URL, &FileSize);
    if (FileContent)
    {
        FT_Face Face;

        if (FT_New_Memory_Face(Instance->Library, FileContent, FileSize, 0, &Face) == 0)
        {
            Result = (font *) AllocateMemory(sizeof(*Result));
            Result->Face = Face;
            Result->FileContent = FileContent;
        }
        else
        {
            // FT_New_Memory_Face failed
            DeallocateMemory(FileContent);
        }
    }
    else
    {
        // ReadEntireFile failed
    }

    return Result;
}

static void
UnloadFont(font **FontPtr)
{
    font *Font = *FontPtr;
    FT_Done_Face(Font->Face);
    DeallocateMemory(Font->FileContent);
    DeallocateMemory(Font);
    *FontPtr = 0;
}
