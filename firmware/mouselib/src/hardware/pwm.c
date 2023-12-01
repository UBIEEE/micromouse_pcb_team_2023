#include <micromouse/hardware/pwm.h>

result_t pwm_set_freq_duty_cycle(uint slice_num, uint channel, uint32_t freq, uint32_t duty_cycle) {
  uint32_t divider16 =
      MICROMOUSE_CLOCK_FREQ_HZ / freq / 4096 + (MICROMOUSE_CLOCK_FREQ_HZ % (freq * 4096) != 0);

  if (divider16 / 16 == 0) {
    divider16 = 16;
  }

  uint32_t wrap = MICROMOUSE_CLOCK_FREQ_HZ * 16 / divider16 / freq - 1;

  pwm_set_clkdiv_int_frac(slice_num, divider16 / 16, divider16 & 0xF);
  pwm_set_wrap(slice_num, wrap);
  pwm_set_chan_level(slice_num, channel, wrap * duty_cycle / 100);

  return RESULT_OK;
}

