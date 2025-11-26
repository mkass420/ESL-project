#include <stdbool.h>
#include <stdint.h>
#include "nrf_gpio.h"
#include "nrfx_systick.h"
#include "pwm.h"

/* Definitions of pins used in project */
#define SW1 NRF_GPIO_PIN_MAP(1, 6)
#define LED1 NRF_GPIO_PIN_MAP(0, 6)
#define LED2_R NRF_GPIO_PIN_MAP(0, 8)
#define LED2_G NRF_GPIO_PIN_MAP(1, 9)
#define LED2_B NRF_GPIO_PIN_MAP(0, 12)

#define LED_NUMBER 4
/* Store led pins for easy iteration */
const uint32_t led_pins[LED_NUMBER] = {LED1, LED2_R, LED2_G, LED2_B};
/* Store how many times should led blink based on DEVICE ID */
const int blink_count[LED_NUMBER] = {6, 5, 9, 3};

extern const uint32_t led_pins[LED_NUMBER];
extern const int blink_count[LED_NUMBER];
extern blinking_led_t* blinking_led_pointers[LED_NUMBER];

pwm_pin_t* pwm_pin_pointers[LED_NUMBER] = {&led1, &led2_r, &led2_g, &led2_b};

void configure_leds(){
    for(size_t i = 0; i < LED_NUMBER; i++){
        pwm_init(pwm_pin_pointers[i], led_pins[i], 0, 1000, true);
    }
}


/**
 * @brief Function for application main entry.
 */
int main(void){
    configure_leds();
    while(true){
        for(size_t i = 0; i < LED_NUMBER; i++){
            for(int j = 0; j < blink_count[i]; j++){
                pwm_poll(pwm_pin_pointers[i]);
                
            }
        }
    }
}

/**
 *@}
 **/
