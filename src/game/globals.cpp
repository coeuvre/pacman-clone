// Memory
allocate_memory_fn *AllocateMemory;
reallocate_memory_fn *ReallocateMemory;
deallocate_memory_fn *DeallocateMemory;

// File
read_entire_file_fn *ReadEntireFile;

// Renderer
get_render_context_fn *GetRenderContext;
load_texture_fn *LoadTexture;

// Input
get_input_fn *GetInput;

static void
InitGlobals(const game_dependencies *Dependencies)
{
    AllocateMemory = Dependencies->AllocateMemory;
    ReallocateMemory = Dependencies->ReallocateMemory;
    DeallocateMemory = Dependencies->DeallocateMemory;

    ReadEntireFile = Dependencies->ReadEntireFile;

    GetRenderContext = Dependencies->GetRenderContext;
    LoadTexture = Dependencies->LoadTexture;

    GetInput = Dependencies->GetInput;
}
