#ifndef __MICROMOUSE_STD_H__
#define __MICROMOUSE_STD_H__

// Appease the pico sdk by letting it do unsafe things.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hardware/irq.h>
#include <hardware/pwm.h>
#include <hardware/spi.h>
#include <pico/multicore.h>
#include <pico/mutex.h>
#include <pico/stdlib.h>

#pragma GCC diagnostic pop

#endif // __MICROMOUSE_STD_H__
