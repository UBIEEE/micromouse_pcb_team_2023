#pragma once

#include <mouse_control/picosdk.hpp>
#include <mouse_control/std.hpp>

namespace hardware {

class MA730 {
  pico::spi_inst_t* m_spi;
  const uint8_t m_cs_pin;

public:
  static constexpr float INVALID_ANGLE = -1.f;

  //
  // Initializes a MA730 encoder.
  // The program will halt if the device could not be initialized.
  //
  // Important: The SPI instance must be initialized prior to calling this!
  //
  MA730(pico::spi_inst_t* spi, uint8_t cs_pin);

  //
  // Reads the angle from the encoder in degrees, [0, 360).
  // The program will halt if the angle could not be read.
  //
  float read_angle() const;
};

} // namespace hardware

