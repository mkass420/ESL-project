#include "button.h"
#include "hsv.h"
#include "pwm.h"
#include "defines.h"
#include "nvmc.h"

#include "app_error.h"
#include "app_timer.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"
#include "nrfx_clock.h"
#include "sdk_errors.h"

#define DEBOUNCE_TICKS APP_TIMER_TICKS(DEBOUNCE_MS)
#define DOUBLE_CLICK_TICKS APP_TIMER_TICKS(DOUBLE_CLICK_MS)
#define HOLD_TICKS APP_TIMER_TICKS(HOLD_MS)
#define COLOR_UPDATE_TICKS APP_TIMER_TICKS(COLOR_UPDATE_MS) 

static volatile button_state_t button_state = BUTTON_STATE_RELEASED;

extern volatile input_mode_t mode;
volatile bool hold = false; 
volatile bool debounce = false;
volatile static int clicks = 0;
extern hsv_color_t current_hsv;

APP_TIMER_DEF(debounce_timer_id);
APP_TIMER_DEF(double_click_timer_id);
APP_TIMER_DEF(hold_timer_id);
APP_TIMER_DEF(color_update_timer_id);

static void color_update_timer_handler(void* p_context){
    cycle_hsv(mode, &current_hsv);
    rgb_color_normalized_t rgb = hsv_to_rgb(current_hsv);
    pwm_rgb_led_set_color(rgb);
}

static void hold_timer_handler(void* p_context){
    // Переход в состояние удержания
    if(button_state == BUTTON_STATE_PRESSED){
        button_state = BUTTON_STATE_HOLD;
        hold = true;
        clicks = 0;
        app_timer_stop(double_click_timer_id);
        app_timer_start(color_update_timer_id, COLOR_UPDATE_TICKS, NULL);
    }
}

static void debounce_timer_handler(void* p_context){
    debounce = false;
}

static void double_click_timer_handler(void* p_context){
    if(clicks > 1){
        mode = (mode + 1) % MODE_COUNT;
        if(led_playback_handlers[mode]) {
            led_playback_handlers[mode]();
        }
        if(mode == MODE_NO_INPUT){
            write_color(&current_hsv);
        }
    }
    clicks = 0;
}

static void button_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action){
    bool pin_state = nrf_gpio_pin_read(pin);
    
    if(debounce) return;
    
    app_timer_start(debounce_timer_id, DEBOUNCE_TICKS, NULL);
    debounce = true;
    
    switch(button_state){
        case BUTTON_STATE_RELEASED:
            if(pin_state == SW1_PRESSED){
                button_state = BUTTON_STATE_PRESSED;
                clicks++;
                if(clicks == 1){
                    app_timer_start(double_click_timer_id, DOUBLE_CLICK_TICKS, NULL);
                }
                app_timer_start(hold_timer_id, HOLD_TICKS, NULL);
            }
            break;
            
        case BUTTON_STATE_PRESSED:
            if(pin_state == SW1_RELEASED){
                button_state = BUTTON_STATE_RELEASED;
                app_timer_stop(hold_timer_id);
                hold = false;
            }
            break;
            
        case BUTTON_STATE_HOLD:
            if(pin_state == SW1_RELEASED){
                button_state = BUTTON_STATE_RELEASED;
                hold = false;
                app_timer_stop(color_update_timer_id);
            }
            break;
    }
}

void timers_init(){
    ret_code_t err_code;
    err_code = nrfx_clock_init(NULL);
    APP_ERROR_CHECK(err_code);
    nrfx_clock_lfclk_start();
    app_timer_init();
    
    err_code = app_timer_create(&debounce_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                debounce_timer_handler);
    APP_ERROR_CHECK(err_code);
    
    err_code = app_timer_create(&double_click_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                double_click_timer_handler);
    APP_ERROR_CHECK(err_code);
    
    err_code = app_timer_create(&hold_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                hold_timer_handler);
    APP_ERROR_CHECK(err_code);
    
    err_code = app_timer_create(&color_update_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                color_update_timer_handler);
    APP_ERROR_CHECK(err_code);
}

void button_init(){
    ret_code_t err_code;
    err_code = nrfx_gpiote_init();
    APP_ERROR_CHECK(err_code);
    
    nrfx_gpiote_in_config_t button_config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    button_config.pull = NRF_GPIO_PIN_PULLUP;
    
    err_code = nrfx_gpiote_in_init(SW1, &button_config, button_handler);
    APP_ERROR_CHECK(err_code);
    
    nrfx_gpiote_in_event_enable(SW1, true);
}