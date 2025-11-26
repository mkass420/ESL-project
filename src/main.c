#include "led.h"
#include "pwm.h"
#include "button.h"
#include "defines.h"

#include <stdbool.h>
#include <stdint.h>
#include "nrfx_systick.h"
//#include "nrf_log.h"
//#include "nrf_log_ctrl.h"
//#include "nrf_log_default_backends.h"
//#include "nrf_log_backend_usb.h"

extern const uint32_t led_pins[LED_NUMBER];
extern const int blink_count[LED_NUMBER];
extern blinking_led_t* blinking_led_pointers[LED_NUMBER];

volatile bool freezed = true;

// void setup_logging(){
//     APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
//     NRF_LOG_DEFAULT_BACKENDS_INIT();
//     NRF_LOG_INFO("Application started.");
//     LOG_BACKEND_USB_PROCESS();
//     NRF_LOG_PROCESS();
//     NRF_LOG_FLUSH();
// }

/**
 * @brief Function for application main entry.
 */
int main(void){
    //setup_logging();
    button_init();
    timers_init();
    configure_leds();
    
    size_t cur_led_idx = 0; // Store led index outside loop to continue from last blinked led
    int cur_led_blinks = 0; // Store led blink count outside loop so that count doesn't reset with button release
    blinking_led_t* cur_led_ptr = blinking_led_pointers[cur_led_idx];
    nrfx_systick_state_t freeze_timestamp = {0};
    nrfx_systick_state_t start_timestamp = {0};
    
    while (true){
        while(!freezed){
            if(freeze_timestamp.time != 0){ // Add time when color was freezed to correctly blink
                advance_timers(&freeze_timestamp, &start_timestamp, cur_led_ptr);
                freeze_timestamp.time = 0;
            }
            blink_led_poll(cur_led_ptr);
            if(cur_led_ptr->hasBlinked){
                cur_led_blinks++;
                cur_led_ptr->hasBlinked = false;
            }
            if(cur_led_blinks == blink_count[cur_led_idx]){ // Manually iterate blinks and leds
                cur_led_idx = (cur_led_idx + 1) % LED_NUMBER;
                cur_led_blinks = 0;
                cur_led_ptr = blinking_led_pointers[cur_led_idx];
            }
            nrfx_systick_get(&start_timestamp);
        }
        if(freezed){
            pwm_poll(&cur_led_ptr->pwm_pin);
            nrfx_systick_get(&freeze_timestamp);
        }
    }
}

/**
 *@}
 **/
