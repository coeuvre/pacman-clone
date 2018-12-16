#ifndef GAME_H
#define GAME_H

#include "platform.h"

extern const platform *GlobalPlatform;

inline PLATFORM_ALLOCATE_MEMORY(PlatformAllocateMemory)
{
    return GlobalPlatform->AllocateMemory(Size);
}

inline PLATFORM_REALLOCATE_MEMORY(PlatformReallocateMemory)
{
    return GlobalPlatform->ReallocateMemory(Pointer, NewSize);
}

inline PLATFORM_DEALLOCATE_MEMORY(PlatformDeallocateMemory)
{
    return GlobalPlatform->DeallocateMemory(Pointer);
}

inline PLATFORM_READ_ENTIRE_FILE(PlatformReadEntireFile)
{
    return GlobalPlatform->ReadEntireFile(URL, FileSize);
}

#endif // GAME_H