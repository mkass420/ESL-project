#include "button.h"
#include "defines.h"

#include "app_error.h"
#include "app_timer.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"
#include "nrfx_clock.h"
//#include "nrf_drv_clock.h"
#include "sdk_errors.h"

#define DEBOUNCE_TICKS APP_TIMER_TICKS(DEBOUNCE_MS)
#define DOUBLE_CLICK_TICKS APP_TIMER_TICKS(DOUBLE_CLICK_MS)

extern volatile bool freezed;
volatile bool debounce = false;
volatile static int clicks = 0;

APP_TIMER_DEF(debounce_timer_id);
APP_TIMER_DEF(double_click_timer_id);


static void debounce_timer_handler(void* p_context){
    debounce = false;
}

static void double_click_timer_handler(void* p_context){
    if(clicks > 1) freezed = !freezed;
    clicks = 0;
}

static void button_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action){
    if(!debounce && nrf_gpio_pin_read(pin) == SW1_PRESSED){
        app_timer_start(debounce_timer_id, DEBOUNCE_TICKS, NULL);
        clicks++;
        debounce = true;
        if (clicks == 1) { 
            app_timer_start(double_click_timer_id, DOUBLE_CLICK_TICKS, NULL);
        }
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