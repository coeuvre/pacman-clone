#include "profiler/profiler.h"

ProfileBlockWatcher::~ProfileBlockWatcher()
{
    PROFILE_CLOSE_BLOCK;
}

profile_context *GlobalProfileContext;

#define ProfileFrameCountMax 1024
#define ProfileBlockCountMaxPerFrame 40960

static profile_context *
LoadProfileContext()
{
    profile_context *Context = (profile_context *) AllocateMemory(sizeof(*Context));
    Context->PerformanceFrequency = GetPerformanceFrequency();
    Context->FrameCountMax = ProfileFrameCountMax;
    Context->FrameCount = 0;
    Context->FrameStartIndex = 0;
    Context->Frames = (profile_frame *) AllocateMemory(sizeof(profile_frame) * Context->FrameCountMax);
    Context->CurrentFrameCount = 0;
    Context->CurrentFrame = 0;
    return Context;
}

static profile_frame *
GetCurrentProfileFrame()
{
    profile_context *Context = GlobalProfileContext;
    profile_frame *Result = Context->CurrentFrame;
    return Result;
}


static profile_frame *
GetLastProfileFrame()
{
    profile_context *Context = GlobalProfileContext;

    profile_frame *Result = 0;
    if (Context->CurrentFrame)
    {
        if (Context->FrameCount > 1)
        {
            Result = &Context->Frames[(Context->FrameStartIndex + Context->FrameCount - 2) % Context->FrameCountMax];
        }
    }
    else
    {
        if (Context->FrameCount > 0)
        {
            Result = &Context->Frames[(Context->FrameStartIndex + Context->FrameCount - 1) % Context->FrameCountMax];
        }
    }

    return Result;
}

static void
ProfileBeginFrame()
{
    if (!GlobalProfileContext)
    {
        GlobalProfileContext = LoadProfileContext();
    }

    profile_context *Context = GlobalProfileContext;

    Assert(Context->CurrentFrame == 0);

    if (Context->FrameCount < Context->FrameCountMax)
    {
        Context->CurrentFrame = &Context->Frames[Context->FrameCount++];
        Context->CurrentFrame->FrameCount = 0;
        Context->CurrentFrame->BlockCountMax = ProfileBlockCountMaxPerFrame;
        Context->CurrentFrame->BlockCount = 0;
        Context->CurrentFrame->Blocks = (profile_block *) AllocateMemory(sizeof(profile_block) * Context->CurrentFrame->BlockCountMax);
        Context->CurrentFrame->RootBlock = &Context->CurrentFrame->Blocks[Context->CurrentFrame->BlockCount++];
    }
    else
    {
        Context->CurrentFrame = &Context->Frames[(Context->FrameStartIndex + Context->FrameCount) % Context->FrameCountMax];
        Context->FrameStartIndex = (Context->FrameStartIndex + 1) % Context->FrameCountMax;
        Context->CurrentFrame->BlockCount = 0;
        Context->CurrentFrame->RootBlock= &Context->CurrentFrame->Blocks[Context->CurrentFrame->BlockCount++];
    }

    profile_frame *CurrentFrame = Context->CurrentFrame;
    Assert(CurrentFrame->FrameCount == 0 || CurrentFrame->FrameCount + ProfileFrameCountMax == Context->CurrentFrameCount + 1);
    CurrentFrame->PerformanceFrequency = Context->PerformanceFrequency;
    CurrentFrame->FrameCount = ++Context->CurrentFrameCount;
    CurrentFrame->CurrentOpenBlock = Context->CurrentFrame->RootBlock;

    profile_block *CurrentOpenBlock = CurrentFrame->CurrentOpenBlock;
    *CurrentOpenBlock = {};
    CurrentOpenBlock->BeginCounter = GetPerformanceCounter();
    CurrentOpenBlock->EndCounter = CurrentFrame->CurrentOpenBlock->BeginCounter;
}

static void
ProfileEndFrame()
{
    profile_context *Context = GlobalProfileContext;
    profile_frame *CurrentFrame = Context->CurrentFrame;
    Assert(CurrentFrame->RootBlock == CurrentFrame->CurrentOpenBlock);
    CurrentFrame->CurrentOpenBlock->EndCounter = GetPerformanceCounter();
    uint64_t FrameCostCounter = CurrentFrame->CurrentOpenBlock->EndCounter - CurrentFrame->CurrentOpenBlock->BeginCounter;
    CurrentFrame->CurrentOpenBlock->FrameTime = FrameCostCounter * 1000.0F / CurrentFrame->PerformanceFrequency;
    Context->CurrentFrame = 0;
}

static void
ProfileOpenBlock(const char *FileName, int32_t LineNumber, const char *FunctionName, const char *BlockName)
{
    profile_frame *CurrentFrame = GetCurrentProfileFrame();
    profile_block *CurrentOpenBlock = CurrentFrame->CurrentOpenBlock;
    Assert(CurrentFrame->BlockCount < CurrentFrame->BlockCountMax);
    profile_block *NewOpenBlock = &CurrentFrame->Blocks[CurrentFrame->BlockCount++];
    NewOpenBlock->FileName = FileName;
    NewOpenBlock->LineNumber = LineNumber;
    NewOpenBlock->FunctionName = FunctionName;
    NewOpenBlock->BlockName = BlockName;
    NewOpenBlock->BeginCounter = GetPerformanceCounter();
    NewOpenBlock->EndCounter = NewOpenBlock->BeginCounter;
    NewOpenBlock->FrameTime = 0;
    NewOpenBlock->Parent = CurrentOpenBlock;
    NewOpenBlock->FirstChild = 0;
    NewOpenBlock->LastChild = 0;
    NewOpenBlock->Next = 0;

    if (CurrentOpenBlock->LastChild)
    {
        CurrentOpenBlock->LastChild->Next = NewOpenBlock;
        CurrentOpenBlock->LastChild = NewOpenBlock;
    }
    else
    {
        CurrentOpenBlock->FirstChild = NewOpenBlock;
        CurrentOpenBlock->LastChild = NewOpenBlock;
    }

    CurrentFrame->CurrentOpenBlock = NewOpenBlock;
}

static void
ProfileCloseBlock()
{
    profile_frame *CurrentFrame = GetCurrentProfileFrame();
    profile_block *CurrentOpenBlock = CurrentFrame->CurrentOpenBlock;
    Assert(CurrentOpenBlock != CurrentFrame->RootBlock);
    CurrentOpenBlock->EndCounter = GetPerformanceCounter();
    uint64_t FrameCostCounter = CurrentOpenBlock->EndCounter - CurrentOpenBlock->BeginCounter;
    CurrentOpenBlock->FrameTime = FrameCostCounter * 1000.0F / CurrentFrame->PerformanceFrequency;
    CurrentFrame->CurrentOpenBlock = CurrentOpenBlock->Parent;
}
