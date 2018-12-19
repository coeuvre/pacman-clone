#ifndef DEFS_H
#define DEFS_H

#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#define EXPORT __unused __attribute__((visibility("default")))

#define ArrayCount(a) (sizeof(a) / sizeof(a[0]))

#define Assert(e) assert(e)

#define INVALID_CODE_PATH Assert(!"Invalid code path")

#define NOOP

#define KB(x) ((x) * 1024UL)
#define MB(x) (KB(x) * 1024UL)
#define GB(x) (MB(x) * 1024UL)

#endif // DEFS_H
