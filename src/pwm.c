#include "pwm.h"
#include "button.h"
#include "defines.h"

#include "color.h"
#include "nrf_gpio.h"
#include "nrf_pwm.h"
#include "nrfx_pwm.h"

static nrfx_pwm_t pwm_rgb_led = NRFX_PWM_INSTANCE(0);
static nrfx_pwm_t pwm_mode_led = NRFX_PWM_INSTANCE(1);

static nrf_pwm_values_individual_t pwm_rgb_values;

static nrf_pwm_values_common_t pwm_mode_values_off [1]= {0};
static nrf_pwm_values_common_t pwm_mode_values_up[DUTY_SEQUENCE_LENGTH] = {0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
static nrf_pwm_values_common_t pwm_mode_values_down[DUTY_SEQUENCE_LENGTH] = {1000, 900, 800, 700, 600, 500, 400, 300, 200, 100, 0};
static nrf_pwm_values_common_t pwm_mode_values_on [1]= {1000};


static nrf_pwm_sequence_t pwm_rgb_seq = {
    .values.p_individual = &pwm_rgb_values,
    .length = NRF_PWM_VALUES_LENGTH(pwm_rgb_values),
    .repeats = 0,
    .end_delay = 0
};


static nrf_pwm_sequence_t pwm_mode_seq_off = {
    .values.p_common = pwm_mode_values_off,
    .length = 1,
    .repeats = 0,
    .end_delay = 0
};


static nrf_pwm_sequence_t pwm_mode_seq_up_slow = {
    .values.p_common = pwm_mode_values_up,
    .length = DUTY_SEQUENCE_LENGTH,
    .repeats = REPEATS_SLOW,
    .end_delay = 0
};

static nrf_pwm_sequence_t pwm_mode_seq_down_slow = {
    .values.p_common = pwm_mode_values_down,
    .length = DUTY_SEQUENCE_LENGTH,
    .repeats = REPEATS_SLOW,
    .end_delay = 0
};


static nrf_pwm_sequence_t pwm_mode_seq_up_fast = {
    .values.p_common = pwm_mode_values_up,
    .length = DUTY_SEQUENCE_LENGTH,
    .repeats = REPEATS_FAST,
    .end_delay = 0
};
static nrf_pwm_sequence_t pwm_mode_seq_down_fast = {
    .values.p_common = pwm_mode_values_down,
    .length = DUTY_SEQUENCE_LENGTH,
    .repeats = REPEATS_FAST,
    .end_delay = 0
};

static nrf_pwm_sequence_t pwm_mode_seq_on = {
    .values.p_common = pwm_mode_values_on,
    .length = 1,
    .repeats = 0,
    .end_delay = 0
};

void pwm_mode_off(){
    nrfx_pwm_stop(&pwm_mode_led, true);
    nrfx_pwm_simple_playback(&pwm_mode_led, 
                                 &pwm_mode_seq_off, 
                                 1, 
                                 NRFX_PWM_FLAG_LOOP);
}

void pwm_mode_slow(){
    nrfx_pwm_stop(&pwm_mode_led, true);
    nrfx_pwm_complex_playback(&pwm_mode_led, 
                                 &pwm_mode_seq_up_slow,
                                 &pwm_mode_seq_down_slow, 
                                 1, 
                                 NRFX_PWM_FLAG_LOOP);
}

void pwm_mode_fast(){
    nrfx_pwm_stop(&pwm_mode_led, true);
    nrfx_pwm_complex_playback(&pwm_mode_led, 
                                 &pwm_mode_seq_up_fast,
                                 &pwm_mode_seq_down_fast, 
                                 1, 
                                 NRFX_PWM_FLAG_LOOP);
}

void pwm_mode_on(){
    nrfx_pwm_stop(&pwm_mode_led, true);
    nrfx_pwm_simple_playback(&pwm_mode_led, 
                                 &pwm_mode_seq_on, 
                                 1, 
                                 NRFX_PWM_FLAG_LOOP);
}


void pwm_rgb_led_init(){
    nrfx_pwm_config_t pwm_rgb_config = NRFX_PWM_DEFAULT_CONFIG;
    pwm_rgb_config.output_pins[0] = LED2_R;
    pwm_rgb_config.output_pins[1] = LED2_G;
    pwm_rgb_config.output_pins[2] = LED2_B;
    pwm_rgb_config.output_pins[3] = NRFX_PWM_PIN_NOT_USED;
    pwm_rgb_config.load_mode = NRF_PWM_LOAD_INDIVIDUAL;
    
    nrfx_pwm_init(&pwm_rgb_led, &pwm_rgb_config, NULL);
    nrfx_pwm_simple_playback(&pwm_rgb_led, &pwm_rgb_seq, 1, NRFX_PWM_FLAG_LOOP);
}

void pwm_mode_led_init(){
    nrfx_pwm_config_t pwm_mode_config = NRFX_PWM_DEFAULT_CONFIG;
    pwm_mode_config.output_pins[0] = LED1;
    pwm_mode_config.output_pins[1] = NRFX_PWM_PIN_NOT_USED;
    pwm_mode_config.output_pins[2] = NRFX_PWM_PIN_NOT_USED;
    pwm_mode_config.output_pins[3] = NRFX_PWM_PIN_NOT_USED;
    pwm_mode_config.load_mode = NRF_PWM_LOAD_COMMON;
    
    nrfx_pwm_init(&pwm_mode_led, &pwm_mode_config, NULL);
    led_playback_handlers[MODE_NO_INPUT]();
}

void pwm_init(){
    pwm_rgb_led_init();
    pwm_mode_led_init();
}

void pwm_rgb_led_set_color(rgb_color_normalized_t rgb){
    pwm_rgb_values.channel_0 = rgb.r * 1000;
    pwm_rgb_values.channel_1 = rgb.g * 1000;
    pwm_rgb_values.channel_2 = rgb.b * 1000;
    pwm_rgb_values.channel_3 = 0;
    
}