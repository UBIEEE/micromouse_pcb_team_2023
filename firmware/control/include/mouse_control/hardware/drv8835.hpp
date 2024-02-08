#pragma once

#include <mouse_control/picosdk.hpp>
#include <mouse_control/std.hpp>

namespace hardware {

class DRV8835 {
  unsigned m_a_phase_slice_num;
  unsigned m_a_phase_channel;
  uint8_t m_a_enable_pin;

  unsigned m_b_phase_slice_num;
  unsigned m_b_phase_channel;
  uint8_t m_b_enable_pin;

public:
  //
  // Initializes a DRV8835 motor driver.
  // The program will halt if the device could not be initialized.
  //
  DRV8835(uint8_t a_phase_pin, uint8_t a_enable_pin,
          uint8_t b_phase_pin, uint8_t b_enable_pin);

  //
  // Sets the output of the two motors connected to the DRV8835.
  // Percentages should be within the range [-1, 1].
  //
  void set_output(float a_percent, float b_percent);

  void stop();

private:
  //
  // Sets the duty cycle of a PWM channel.
  // Duty cycle should be within the range [0, 100].
  //
  void set_duty_cycle(unsigned slice_num, unsigned channel, uint16_t duty);
};

} // namespace hardware
                       
