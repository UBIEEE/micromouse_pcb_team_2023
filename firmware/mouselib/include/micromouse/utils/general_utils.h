#ifndef __MICROMOUSE_GENERAL_UTILS_H__
#define __MICROMOUSE_GENERAL_UTILS_H__

#include <micromouse/micromouse.h>

#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

#define FLOAT_EQ(x, y) (fabsf((x) - (y)) < 0.0001f)

#endif // __MICROMOUSE_GENERAL_UTILS_H__

