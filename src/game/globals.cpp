// Memory
allocate_memory_fn *AllocateMemory;
reallocate_memory_fn *ReallocateMemory;
deallocate_memory_fn *DeallocateMemory;

// File
read_entire_file_fn *ReadEntireFile;

// Renderer
render_context *RenderContext;
load_texture_fn *LoadTexture;

input *Input;

static void
InitGlobals(const game_dependencies *Dependencies)
{
    AllocateMemory = Dependencies->AllocateMemory;
    ReallocateMemory = Dependencies->ReallocateMemory;
    DeallocateMemory = Dependencies->DeallocateMemory;

    ReadEntireFile = Dependencies->ReadEntireFile;

    RenderContext = Dependencies->RenderContext;
    LoadTexture = Dependencies->LoadTexture;

    Input = Dependencies->Input;
}
