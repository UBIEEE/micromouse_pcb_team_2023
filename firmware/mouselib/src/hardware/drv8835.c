#include <micromouse/hardware/drv8835.h>

#include <micromouse/diagnostics.h>
#include <micromouse/macros.h>

#include <micromouse/hardware/pwm.h>

// Max is 250Hz
#define DRV8835_FREQ_HZ 200

static void set_enable(const drv8835_dev_t* dev, bool a_enable, bool b_enable);

drv8835_dev_t drv8835_init(uint8_t a_phase_pin, uint8_t a_enable_pin,
                           uint8_t b_phase_pin, uint8_t b_enable_pin) {
  drv8835_dev_t dev = {0};

  // APH and BPH

  gpio_set_function(a_phase_pin, GPIO_FUNC_PWM);
  dev.a_phase_slice_num = pwm_gpio_to_slice_num(a_phase_pin);
  dev.a_phase_channel   = pwm_gpio_to_channel(a_phase_pin);

  gpio_set_function(b_phase_pin, GPIO_FUNC_PWM);
  dev.b_phase_slice_num = pwm_gpio_to_slice_num(b_phase_pin);
  dev.b_phase_channel   = pwm_gpio_to_channel(b_phase_pin);

  pwm_set_enabled(dev.a_phase_slice_num, true);
  pwm_set_enabled(dev.b_phase_slice_num, true);

  // AEN and BEN

  dev.a_enable_pin = a_enable_pin;
  dev.b_enable_pin = b_enable_pin;

  gpio_init(a_enable_pin);
  gpio_set_dir(a_enable_pin, GPIO_OUT);
  gpio_put(a_enable_pin, 0);

  gpio_init(b_enable_pin);
  gpio_set_dir(b_enable_pin, GPIO_OUT);
  gpio_put(b_enable_pin, 0);

  // Set initial output to 0.
  (void)drv8835_set_output(&dev, 0.f, 0.f);

  return dev;
}

result_t drv8835_set_output(drv8835_dev_t* dev, float a_percent,
                            float b_percent) {
  if (!dev) DIAG_REPORT_INVALID_ARG();

  // Clamp to [-1, 1]
  a_percent = CLAMP(a_percent, -1.f, 1.f);
  b_percent = CLAMP(b_percent, -1.f, 1.f);

  // Set duty cycle.

  // 0-100 percent duty cycle (0% -> full reverse, 100% -> full forward).
  uint32_t a_duty_cycle = (uint32_t)((a_percent + 1.f) * 50.f);
  uint32_t b_duty_cycle = (uint32_t)((b_percent + 1.f) * 50.f);

  result_t result = RESULT_OK;

  result |=
      pwm_set_freq_duty_cycle(dev->a_phase_slice_num, dev->a_phase_channel,
                              DRV8835_FREQ_HZ, a_duty_cycle);
  result |=
      pwm_set_freq_duty_cycle(dev->b_phase_slice_num, dev->b_phase_channel,
                              DRV8835_FREQ_HZ, b_duty_cycle);

  bool a_enable = !FLOAT_EQ(a_percent, 0.f);
  bool b_enable = !FLOAT_EQ(b_percent, 0.f);

  set_enable(dev, a_enable, b_enable);

  return result;
}

static void set_enable(const drv8835_dev_t* dev, bool a_enable, bool b_enable) {
  gpio_put(dev->a_enable_pin, a_enable);
  gpio_put(dev->b_enable_pin, b_enable);
}

