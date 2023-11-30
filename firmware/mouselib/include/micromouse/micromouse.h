#ifndef __MICROMOUSE_H__
#define __MICROMOUSE_H__

// Turn off -Wpedantic for Pico SDK headers.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hardware/spi.h>
#include <pico/stdlib.h>

#pragma GCC diagnostic pop

#include <micromouse/result.h>

#define M_PI_180 0.01745329252f

#endif // __MICROMOUSE_H__
