#ifndef PWM_H
#define PWM_H
#include "color.h"
#include <stdbool.h>


typedef void (*led_mode_handler_t)(void);

void pwm_mode_off();
void pwm_mode_slow();
void pwm_mode_fast();
void pwm_mode_on();

static const led_mode_handler_t led_playback_handlers[] = {
    pwm_mode_off, 
    pwm_mode_slow,
    pwm_mode_fast,
    pwm_mode_on
};

void pwm_rgb_led_init();
void pwm_mode_led_init();
void pwm_init();

void pwm_rgb_led_set_color(rgb_color_normalized_t rgb);

#endif