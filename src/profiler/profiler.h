#ifndef PROFILER_PROFILER_H
#define PROFILER_PROFILER_H

struct profile_block
{
    const char *FileName;
    int32_t LineNumber;
    const char *FunctionName;

    uint64_t BeginCounter;
    uint64_t EndCounter;
    float FrameTime;

    profile_block *Parent;
    profile_block *FirstChild;
    profile_block *LastChild;

    profile_block *Next;
};

struct profile_frame
{
    uint64_t FrameCount;
    uint64_t PerformanceFrequency;

    uint32_t BlockCountMax;
    uint32_t BlockCount;
    profile_block *Blocks;

    profile_block *RootBlock;
    profile_block *CurrentOpenBlock;
};

struct profile_context
{
    uint64_t PerformanceFrequency;

    uint32_t FrameCountMax;
    uint32_t FrameStartIndex;
    uint32_t FrameCount;
    profile_frame *Frames;

    uint64_t CurrentFrameCount;
    profile_frame *CurrentFrame;
};

static profile_frame *
GetCurrentProfileFrame();

static profile_frame *
GetLastProfileFrame();

static void
ProfileBeginFrame();

static void
ProfileEndFrame();

#define PROFILE_BEGIN_FRAME ProfileBeginFrame()
#define PROFILE_END_FRAME ProfileEndFrame()

static void
ProfileOpenBlock(const char *FileName, int32_t LineNumber, const char *FunctionName);

static void
ProfileCloseBlock();

#define PROFILE_OPEN_BLOCK ProfileOpenBlock(__FILE__, __LINE__, __func__)
#define PROFILE_CLOSE_BLOCK ProfileCloseBlock()

#endif // PROFILER_PROFILER_H
