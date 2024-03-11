#include <mouse_control/subsystems/illumination.hpp>

#include <mouse_control/constants.hpp>
#include <mouse_control/picosdk.hpp>

static constexpr auto BLINK_PERIOD =
    Constants::Runtime::LOOP_FREQ_PRIMARY_HZ / 2; // number of 20ms ticks.

Illumination::Illumination() {
  std::memset(m_led_states, 0, sizeof(m_led_states));

  for (uint8_t i = 0; i < LED_COUNT; i++) {
    const auto& led_pin = Constants::IO::LEDS[i];
    pico::gpio_init(led_pin);
    pico::gpio_set_dir(led_pin, GPIO_OUT);
    pico::gpio_put(led_pin, 0);
  }
}

void Illumination::process() {
  const bool blink_on =
      m_iter++ % Constants::Runtime::LOOP_FREQ_PRIMARY_HZ < BLINK_PERIOD;

  // Don't update if not changed.
  if (blink_on == m_blink_on) return;
  m_blink_on = blink_on;

  for (uint8_t i = 0; i < LED_COUNT; i++) {
    if (m_led_states[i] != LedState::BLINK) continue;
    set_led_impl(Led(i), m_blink_on);
  }
}

void Illumination::set_led(Led led, LedState state) {
  // Set the state of the LED.
  m_led_states[static_cast<uint8_t>(led)] = state;

  // Set it on/off.
  bool on = state == LedState::ON;
  if (state == LedState::BLINK) {
    on = m_blink_on;
  }
  set_led_impl(led, on);
}

void Illumination::set_led_impl(Led led, bool on) {
  pico::gpio_put(Constants::IO::LEDS[uint8_t(led)], on);
}
