#pragma once

#include <mouse_control/std.hpp>
#include <mouse_control/subsystems/subsystem.hpp>

//
// Subsystem for controlling the five LEDs on the mouse.
//
class Illumination : public Subsystem {
  uint64_t m_iter = 0;
  bool m_blink_on = false;

public:
  static constexpr uint8_t LED_COUNT = 5;

  Illumination();

  void process() override;

  enum class Led : uint8_t {
    RED = 0,
    GREEN,
    BLUE,
    ORANGE,
    YELLOW,
  };

  enum class LedState : uint8_t {
    OFF = 0,
    BLINK,
    ON,
  };

  void set_led(Led led, LedState state);

  // TODO: Cool effects?

private:
  void set_led_impl(Led led, bool on);

private:
  LedState m_led_states[LED_COUNT];
};
