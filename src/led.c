#include "led.h"
#include "pwm.h"
#include "defines.h"

#include "nrf_gpio.h"
#include "nrfx_systick.h"
#include <stdbool.h>
#include <stdint.h>

/* Store led pins for easy iteration */
const uint32_t led_pins[LED_NUMBER] = {LED1, LED2_R, LED2_G, LED2_B};
/* Store how many times should led blink based on DEVICE ID */
const int blink_count[LED_NUMBER] = {6, 5, 9, 3};

blinking_led_t led1;
blinking_led_t led2_r;
blinking_led_t led2_g;
blinking_led_t led2_b;
blinking_led_t* blinking_led_pointers[LED_NUMBER] = {&led1, &led2_r, &led2_g, &led2_b};

void led_init(blinking_led_t* p, uint32_t pin){
    pwm_init(&p->pwm_pin, pin, 0, 1000, true);
    p->current_brightness = 0;
    p->isIncreasing = true;
    nrfx_systick_get(&p->blink_systick_state);
}

void configure_leds(){
    nrfx_systick_init();
    for(size_t i = 0; i < LED_NUMBER; i++){
        led_init(blinking_led_pointers[i], led_pins[i]);
        blink_led_poll(blinking_led_pointers[i]);
    }
}

void blink_led_set_brightness(blinking_led_t* p, int brightness){
    p->current_brightness = brightness;
    pwm_set_duty(&p->pwm_pin, brightness);
}

void blink_led_poll(blinking_led_t* p){
    pwm_poll(&p->pwm_pin);
    if(nrfx_systick_test(&p->blink_systick_state, BLINK_DELTA_US)){
        nrfx_systick_get(&p->blink_systick_state);
        int new_brightness = p->isIncreasing ? p->current_brightness + BLINK_DUTY_DELTA : p->current_brightness - BLINK_DUTY_DELTA;
        blink_led_set_brightness(p, new_brightness);
        if(p->current_brightness == 100) {
            p->isIncreasing = false;
        } else if(p->current_brightness == 0 && !p->isIncreasing) {
            p->hasBlinked = true;
            p->isIncreasing = true;
        }
    }
}

void advance_timers(nrfx_systick_state_t* freeze, nrfx_systick_state_t* start, blinking_led_t* led){
    uint32_t time_dif = freeze->time - start->time;
    led->blink_systick_state.time += time_dif;
}