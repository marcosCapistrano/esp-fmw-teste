#ifndef PWM_H
#define PWM_H

#include "driver/mcpwm.h"

typedef struct pwm* pwm_t;

struct pwm {
	const char* name;
	mcpwm_unit_t mcpwm_unit;
	mcpwm_timer_t timer_num;
};

pwm_t pwm_init(const char *name, mcpwm_unit_t mcpwm_unit, mcpwm_timer_t timer_num, mcpwm_io_signals_t io_signal, int gpio_num);

void pwm_set_duty(pwm_t pwm, int value);

#endif