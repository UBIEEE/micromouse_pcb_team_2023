#include <mouse_control/hardware/drv8835.hpp>

#include <mouse_control/constants.hpp>
#include <mouse_control/macros.hpp>

// Max is 250Hz
static constexpr uint16_t DRV8835_FREQ_HZ = 200;

using namespace hardware;

DRV8835::DRV8835(uint8_t a_phase_pin, uint8_t a_enable_pin, uint8_t b_phase_pin,
                 uint8_t b_enable_pin) {

  pico::gpio_set_function(a_phase_pin, pico::GPIO_FUNC_PWM);
  m_a_phase_slice_num = pico::pwm_gpio_to_slice_num(a_phase_pin);
  m_a_phase_channel   = pico::pwm_gpio_to_channel(a_phase_pin);

  pico::gpio_set_function(b_phase_pin, pico::GPIO_FUNC_PWM);
  m_b_phase_slice_num = pico::pwm_gpio_to_slice_num(b_phase_pin);
  m_b_phase_channel   = pico::pwm_gpio_to_channel(b_phase_pin);

  pico::pwm_set_enabled(m_a_phase_slice_num, true);
  pico::pwm_set_enabled(m_b_phase_slice_num, true);

  // AEN and BEN

  m_a_enable_pin = a_enable_pin;
  m_b_enable_pin = b_enable_pin;

  pico::gpio_init(a_enable_pin);
  pico::gpio_set_dir(a_enable_pin, GPIO_OUT);
  pico::gpio_put(a_enable_pin, 0);

  pico::gpio_init(b_enable_pin);
  pico::gpio_set_dir(b_enable_pin, GPIO_OUT);
  pico::gpio_put(b_enable_pin, 0);

  // Stop the motors.
  stop();
}

//
// Sets the output of the two motors connected to the DRV8835.
// Percentages should be within the range [-1, 1].
//
void DRV8835::set_output(float a_percent, float b_percent) {
  // [-1, 1]
  a_percent = std::clamp(a_percent, -1.f, 1.f);
  b_percent = std::clamp(b_percent, -1.f, 1.f);

  // [-1, 1] -> [0, 100]
  const uint16_t a_duty = static_cast<uint16_t>((a_percent + 1.f) * 50.f);
  const uint16_t b_duty = static_cast<uint16_t>((b_percent + 1.f) * 50.f);

  set_duty_cycle(m_a_phase_slice_num, m_a_phase_channel, a_duty);
  set_duty_cycle(m_b_phase_slice_num, m_b_phase_channel, b_duty);

  const bool a_enable = a_percent != 0.f;
  const bool b_enable = b_percent != 0.f;

  pico::gpio_put(m_a_enable_pin, a_enable);
  pico::gpio_put(m_b_enable_pin, b_enable);
}

void DRV8835::stop() {
  // Stop the motors.
  set_output(0.f, 0.f);
}

void DRV8835::set_duty_cycle(unsigned slice_num, unsigned channel,
                             uint16_t duty) {
  uint32_t divider16 = CLOCK_FREQ_HZ / DRV8835_FREQ_HZ / 4096 +
                       (CLOCK_FREQ_HZ % (DRV8835_FREQ_HZ * 4096) != 0);

  if (divider16 / 16 == 0) {
    divider16 = 16;
  }

  const uint32_t wrap = CLOCK_FREQ_HZ * 16 / divider16 / DRV8835_FREQ_HZ - 1;

  pico::pwm_set_clkdiv_int_frac(slice_num, divider16 / 16, divider16 & 0xF);
  pico::pwm_set_wrap(slice_num, wrap);
  pico::pwm_set_chan_level(slice_num, channel, wrap * duty / 100);
}

