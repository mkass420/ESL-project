#include <stdint.h>
#include "cmsis_gcc.h"
#include "hsv.h"
#include "pwm.h"
#include "button.h"
#include "defines.h"

#include <stdbool.h>
#include "nrf_pwr_mgmt.h"

volatile input_mode_t mode = MODE_NO_INPUT;
extern volatile bool hold;
hsv_color_t current_hsv = {(DEVICE_ID % 100) * 360 / 100, 100, 100};

/**
 * @brief Function for application main entry.
 */
int main(void){
    button_init();
    timers_init();
    pwm_init();

    rgb_color_normalized_t rgb = hsv_to_rgb(current_hsv);
    pwm_rgb_led_set_color(rgb);

    while (true){
        __SEV();
        __WFE();
        __WFE();
    }
}

/**
 *@}
 **/
