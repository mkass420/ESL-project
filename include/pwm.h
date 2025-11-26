#ifndef PWM_H
#define PWM_H
#include "nrfx_systick.h"
#include <stdint.h>
#include <stdbool.h>

#define PWM_FREQ_TO_TIME_PERIOD_US(freq) (10e6 / (freq))
#define PWM_DUTY_TO_TIME_ON(duty, time) (uint32_t) ((((float) duty) / 100) * (time))

typedef struct{
    uint32_t time_period_us;
    uint32_t time_on_us;
    uint32_t pin;
    bool active_low;
    nrfx_systick_state_t systick_state;
    bool is_on;
} pwm_pin_t;

void pwm_init(pwm_pin_t* p, uint32_t pin, int duty_cycle, uint32_t frequency, bool active_low);

void pwm_set_duty(pwm_pin_t* p, int duty_cycle);

void pin_write(pwm_pin_t* p, bool state);

void pwm_poll(pwm_pin_t* p);

#endif