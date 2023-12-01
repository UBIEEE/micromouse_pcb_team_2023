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
#include <hardware/pwm.h>
#include <pico/stdlib.h>

#pragma GCC diagnostic pop

#include <micromouse/result.h>
#include <micromouse/constants.h>

#endif // __MICROMOUSE_H__
