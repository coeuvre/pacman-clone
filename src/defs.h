#ifndef DEFS_H
#define DEFS_H

#include <stddef.h>
#include <stdint.h>

#define EXPORT __unused __attribute__((visibility("default")))

#define ArrayLength(a) (sizeof(a) / sizeof(a[0]))

#endif // DEFS_H
