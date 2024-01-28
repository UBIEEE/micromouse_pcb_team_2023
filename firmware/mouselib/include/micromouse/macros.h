#ifndef __MICROMOUSE_MACROS_H__
#define __MICROMOUSE_MACROS_H__

#include <micromouse/std.h>

#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

#define FLOAT_EQ(x, y) (fabsf((x) - (y)) < 0.0001f)

// Prints the current source location and a message, then halts execution.
#define FATAL_ERROR(TAG, FMT, ...)                                             \
  panic(TAG " [%s:%s:%d] " FMT "\n", __FILE__, __func__, __LINE__,             \
        ##__VA_ARGS__)

#define TAG_INVALID_ARGS "[INVALID ARGS]"
#define TAG_SPI_ERROR    "[SPI ERROR]"

// Fatal error for invalid arguments.
#define INVALID_ARGS() FATAL_ERROR(TAG_INVALID_ARGS, "")

#endif // __MICROMOUSE_MACROS_H__

