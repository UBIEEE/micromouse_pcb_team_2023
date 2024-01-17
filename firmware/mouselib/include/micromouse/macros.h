#ifndef __MICROMOUSE_MACROS_H__
#define __MICROMOUSE_MACROS_H__

#include <micromouse/std.h>

#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

#define FLOAT_EQ(x, y) (fabsf((x) - (y)) < 0.0001f)

#endif // __MICROMOUSE_MACROS_H__

