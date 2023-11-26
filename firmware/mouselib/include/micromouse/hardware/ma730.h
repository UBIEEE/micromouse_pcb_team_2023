#ifndef __MICROMOUSE_MA730_H__
#define __MICROMOUSE_MA730_H__

#include <hardware/spi.h>
#include <micromouse/micromouse.h>

typedef struct ma730_handle_t {
  spi_inst_t* spi;
  uint8_t cs;
} ma730_handle_t;

//
// Initialzes a SPI instance for the MA730 encoder.
// Only call this once per SPI instance.
//
void ma730_init_spi_instance(spi_inst_t* spi);

//
// Initializes the pins for the MA730 encoder.
//
// Important: The SPI instance must be initialized prior to calling this.
//
ma730_handle_t ma730_init(spi_inst_t* spi, uint8_t cs_pin, uint8_t sclk_pin,
                          uint8_t mosi_pin, uint8_t miso_pin);

//
// Reads the angle from the encoder, then converts it to radians (0 to 2π).
//
float ma730_read_angle(ma730_handle_t* handle);

#endif // __MICROMOUSE_MA730_H__
