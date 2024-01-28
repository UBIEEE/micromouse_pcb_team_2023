#ifndef __MICROMOUSE_MA730_H__
#define __MICROMOUSE_MA730_H__

#include <micromouse/std.h>

struct ma730_dev {
  spi_inst_t* spi;
  uint8_t cs;
};
typedef struct ma730_dev ma730_dev_t;

#define MA730_ANGLE_INVALID (-1.f)

//
// Initializes a MA730 encoder.
// The program will halt if the device could not be initialized.
//
// Important: The SPI instance must be initialized prior to calling this!
//
ma730_dev_t ma730_init(spi_inst_t* spi, uint8_t cs_pin);

//
// Reads the angle from the encoder in degrees, [0, 360).
//
// The program will panic if the angle could not be read.
//
float ma730_read_angle(ma730_dev_t* dev);

#endif // __MICROMOUSE_MA730_H__

