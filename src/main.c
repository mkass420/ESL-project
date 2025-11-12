#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"

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

/**
 * @brief Function for turning leds on/off
 */
void write_led(uint32_t pin, bool state){ // Leds are active-low, so inverse their state
    nrf_gpio_pin_write(pin, !state);
}

/**
 * @brief Function for configuring gpio ports for leds and switch
 */
void configure_gpio(){
    nrf_gpio_cfg_input(SW1, NRF_GPIO_PIN_PULLUP);
    for(size_t i = 0; i < LED_NUMBER; i++){
        nrf_gpio_cfg_output(led_pins[i]);
        write_led(led_pins[i], 0);
    }
}

/**
 * @brief Function for application main entry.
 */
int main(void){
    configure_gpio();
    size_t cur_led_idx = 0; // Store led index outside loop to continue from last blinked led
    int cur_led_blinks = 0; // Store led blink count outside loop so that count doesn't reset with button release
    bool cur_led_state = 0; // Store led state so that led saves state when butten is released
    while (true){
        while(nrf_gpio_pin_read(SW1) == 0){ // When button is pressed
            cur_led_state = !cur_led_state;
            write_led(led_pins[cur_led_idx], cur_led_state);
            nrf_delay_ms(500);

            cur_led_blinks += !cur_led_state; // Adds only when led is turned off
            if(cur_led_blinks == blink_count[cur_led_idx]){ // Manually iterate blinks and leds
                cur_led_idx = (cur_led_idx + 1) % 4;
                cur_led_blinks = 0;
            }
        }
    }
}

/**
 *@}
 **/
