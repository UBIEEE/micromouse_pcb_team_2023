#pragma once

#include <mouse_control/picosdk.hpp>
#include <mouse_control/std.hpp>

namespace hardware {

class MA730 {
  pico::spi_inst_t* m_spi;
  const uint8_t m_cs_pin;

  float m_last_angle = 0;
  float m_rotations  = 0.f;

public:
  //
  // Initializes a MA730 encoder.
  // The program will halt if the device could not be initialized.
  //
  // Important: The SPI instance must be initialized prior to calling this!
  //
  MA730(pico::spi_inst_t* spi, uint8_t cs_pin);

  struct Data {
    float distance; // rotations
    float velocity; // rotations per second

    Data apply_coefficient(const float& coefficient) const {
      return Data {distance * coefficient, velocity * coefficient};
    }
  };

  //
  // Reads the accumulated rotations and the velocity of the encoder.
  // dt is the time since the last call to read (in seconds).
  //
  Data read(const float& dt);

  //
  // Resets the accumulated rotations.
  //
  void reset() { m_rotations = 0.f; }

private:
  //
  // Reads the 14-bit angle from the encoder [0, 16384).
  //
  uint16_t read_angle_raw() const;
};

} // namespace hardware
