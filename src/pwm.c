#include "pwm.h"
#include "nrf_gpio.h"
#include "nrfx_systick.h"

void pwm_init(pwm_pin_t* p, uint32_t pin, int duty_cycle, uint32_t frequency, bool active_low){
    nrf_gpio_cfg_output(pin);
    nrfx_systick_init();
    
    uint32_t time_period_us = PWM_FREQ_TO_TIME_PERIOD_US(frequency);
    uint32_t time_on_us = PWM_DUTY_TO_TIME_ON(duty_cycle, time_period_us);
    
    p->time_period_us = time_period_us;
    p->time_on_us = time_on_us;
    p->pin = pin;
    p->active_low = active_low;
    nrfx_systick_get(&p->systick_state);
    
    pin_write(p, 1); // Always start our period with active state
}

void pwm_set_duty(pwm_pin_t* p, int duty_cycle){
    p->time_on_us = PWM_DUTY_TO_TIME_ON(duty_cycle, p->time_period_us);
}

void pin_write(pwm_pin_t* p, bool state){
    nrf_gpio_pin_write(p->pin, (state ^ p->active_low)); // Writes 0 or 1 with respect of active_low (inverse if active_low is true)
}

void pwm_poll(pwm_pin_t* p){
    if(nrfx_systick_test(&p->systick_state, p->time_period_us)){
        nrfx_systick_get(&p->systick_state);
        pin_write(p, 1);
    }
    else if(nrfx_systick_test(&p->systick_state, p->time_on_us)){
        pin_write(p, 0);
    }
}