#ifndef __MICROMOUSE_DRV8835_H__
#define __MICROMOUSE_DRV8835_H__

#include <micromouse/result.h>
#include <micromouse/std.h>

struct drv8835_dev {
  uint a_phase_slice_num;
  uint a_phase_channel;
  uint8_t a_enable_pin;

  uint b_phase_slice_num;
  uint b_phase_channel;
  uint8_t b_enable_pin;
};
typedef struct drv8835_dev drv8835_dev_t;

//
// Initializes a DRV8835 motor driver.
//
drv8835_dev_t drv8835_init(uint8_t a_phase_pin, uint8_t a_enable_pin,
                           uint8_t b_phase_pin, uint8_t b_enable_pin);

//
// Sets the output of the two motors connected to the DRV8835.
// Percentages should be in the range [-1, 1].
//
result_t drv8835_set_output(drv8835_dev_t* dev, float a_percent,
                            float b_percent);

#endif // __MICROMOUSE_DRV8835_H__

