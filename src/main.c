#include <stdint.h>
#include "cmsis_gcc.h"
#include "color.h"
#include "pwm.h"
#include "button.h"
#include "defines.h"
#include "nvmc.h"
#include "cli.h"

#include <stdbool.h>
#include <string.h>
#include "nrf_pwr_mgmt.h"

volatile input_mode_t mode = MODE_NO_INPUT;
extern volatile bool hold;
volatile hsv_color_t current_hsv;
volatile color_palette_t palette;

int main(void){
    if(!read_color(&current_hsv)){
        current_hsv.hue = DEFAULT_HUE;
        current_hsv.sat = DEFAULT_SATURATION;
        current_hsv.val = DEFAULT_VALUE;
    }
    if(!read_palette(&palette)){
        memset((uint8_t*)palette.raw_data, 0, PALETTE_SIZE);
    }
    button_init();
    timers_init();
    pwm_init();
    usbd_init();
    //cli_init();
    //cli_start();

    rgb_color_normalized_t rgb = hsv_to_rgb((hsv_color_t*)&current_hsv);
    pwm_rgb_led_set_color(rgb);

    while (true){
        __SEV();
        __WFE();
        __WFE();
        cli_process();
    }
}

/**
 *@}
 **/
