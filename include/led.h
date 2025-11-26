#ifndef LED_H
#define LED_H
#include "pwm.h"
#include "nrfx_systick.h"

#define MS_TO_US(ms) (ms * 1000)
#define BLINK_TIME_US MS_TO_US(BLINK_TIME_MS)
#define BLINK_DELTA_US (int)(BLINK_TIME_US / (int)(100 / BLINK_DUTY_DELTA))

typedef struct{
    pwm_pin_t pwm_pin;
    int current_brightness;
    bool isIncreasing;
    bool hasBlinked;
    nrfx_systick_state_t blink_systick_state;
} blinking_led_t;


void led_init(blinking_led_t* p, uint32_t pin);

void configure_leds(void);

void blink_led_set_brightness(blinking_led_t* p, int brightness);

void blink_led_poll(blinking_led_t* p);

void advance_timers(nrfx_systick_state_t* freeze, 
                    nrfx_systick_state_t* start, 
                    blinking_led_t* led);

#endif