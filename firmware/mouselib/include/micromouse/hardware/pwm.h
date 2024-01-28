#ifndef __MICROMOUSE_PWM_H__
#define __MICROMOUSE_PWM_H__

#include <micromouse/std.h>

void pwm_set_freq_duty_cycle(uint slice_num, uint channel, uint32_t freq,
                             uint32_t duty_cycle);

#endif // __MICROMOUSE_PWM_H__

