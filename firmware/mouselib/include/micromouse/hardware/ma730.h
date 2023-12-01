#ifndef __MICROMOUSE_MA730_H__
#define __MICROMOUSE_MA730_H__

#include <micromouse/micromouse.h>

struct ma730_dev {
  spi_inst_t* spi;
  uint8_t cs;
};
typedef struct ma730_dev ma730_dev_t;

#define MA730_ANGLE_INVALID (-1.f)

//
// Initializes a MA730 encoder.
//
// Important: The SPI instance must be initialized prior to calling this.
//
result_t ma730_init(ma730_dev_t* dev, spi_inst_t* spi, uint8_t cs_pin);

//
// Reads the angle from the encoder, in radians (0 to 2π).
//
// The angle output parameter is set to MA730_ANGLE_INVALID if the angle could not be read.
//
result_t ma730_read_angle(ma730_dev_t* dev, float* angle);

#endif // __MICROMOUSE_MA730_H__

