#ifndef PLATFORM_PLATFORM_H
#define PLATFORM_PLATFORM_H

static void *AllocateMemory(size_t Size);
static void *ReallocateMemory(void *Pointer, size_t NewSize);
static void DeallocateMemory(void *Pointer);

static uint8_t *ReadEntireFile(const char *URL, size_t *FileSize);

struct input
{
    float DeltaTime;
};

static input *GetInput();

#endif // PLATFORM_PLATFORM_H
