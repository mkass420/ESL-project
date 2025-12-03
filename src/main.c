#include <stdint.h>
#include "cmsis_gcc.h"
#include "hsv.h"
#include "pwm.h"
#include "button.h"
#include "defines.h"
#include "nvmc.h"

#include <stdbool.h>
#include "nrf_pwr_mgmt.h"

volatile input_mode_t mode = MODE_NO_INPUT;
extern volatile bool hold;
hsv_color_t current_hsv;

int main(void){
    if(!read_color(&current_hsv)){
        current_hsv.hue = DEFAULT_HUE;
        current_hsv.sat = DEFAULT_SATURATION;
        current_hsv.val = DEFAULT_VALUE;
    }
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
