#ifndef __MICROMOUSE_UTIL_H__
#define __MICROMOUSE_UTIL_H__

#include <math.h>

#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

#define FLOAT_EQ(x, y) (fabsf((x) - (y)) < 0.0001f)
#define DOUBLE_EQ(x, y) (fabs((x) - (y)) < 0.0001)

#endif // __MICROMOUSE_UTIL_H__
