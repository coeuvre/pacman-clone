static void *
AllocateMemory(size_t Size)
{
    return malloc(Size);
}

static void *ReallocateMemory(void *Pointer, size_t NewSize)
{
    return realloc(Pointer, NewSize);
}

static void DeallocateMemory(void *Pointer)
{
    free(Pointer);
}

static render_context *GlobalRenderContext;

static render_context *
GetRenderContext()
{
    return GlobalRenderContext;
}

static input *GlobalInput;

static input *
GetInput()
{
    return GlobalInput;
}

#include "platform/platform_sdl.cpp"