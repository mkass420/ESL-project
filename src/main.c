#include "color.h"
#include "pwm.h"
#include "button.h"
#include "defines.h"
#include "nvmc.h"
#include "log_utils.h"
#include "cli.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "nordic_common.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_log_backend_usb.h"

#include "app_usbd.h"
#include "app_usbd_serial_num.h"

#include "cmsis_gcc.h"
#include "nrf_pwr_mgmt.h"

#include "nrf_delay.h"
#include "nrf_drv_clock.h"
#include "sdk_errors.h"

volatile input_mode_t mode = MODE_NO_INPUT;
extern volatile bool hold;
volatile hsv_color_t current_hsv;
volatile color_palette_t palette;

void clocks_init(){
    nrfx_err_t ret = nrf_drv_clock_init(); 
    if(ret != NRF_SUCCESS && ret != NRF_ERROR_MODULE_ALREADY_INITIALIZED){
        APP_ERROR_CHECK(ret);
    }
    if(!nrf_drv_clock_lfclk_is_running()) nrf_drv_clock_lfclk_request(NULL);
    if(!nrf_drv_clock_hfclk_is_running()) nrf_drv_clock_hfclk_request(NULL);
    
    while(!nrf_drv_clock_lfclk_is_running());
    while(!nrf_drv_clock_hfclk_is_running());
    
    NRF_LOG_INFO("Successfully initialized lfclk and hfclk");
}

void try_read_nvmc(){
    if(read_color(&current_hsv)){
        NRF_LOG_INFO("Successfully read color from memory");
    }
    else{
        current_hsv.hue = DEFAULT_HUE;
        current_hsv.sat = DEFAULT_SATURATION;
        current_hsv.val = DEFAULT_VALUE;
    }
    
    if(!read_palette(&palette)){
        memset((uint8_t*)palette.raw_data, 0, PALETTE_SIZE);
    }
    log_palette((color_palette_t*)&palette);
}

int main(void){
    clocks_init();
    logs_init();

    for (int i = 0; i < 50; i++){ // wait 5 seconds for usb initialization (yeah it's bad, but otherwise it just loses data)
        nrf_delay_ms(100);
        LOG_BACKEND_USB_PROCESS(); 
        NRF_LOG_PROCESS(); 
    }
    
    try_read_nvmc();
    
    button_init();
    timers_init();
    pwm_init();
    
    pwm_rgb_led_set_color(hsv_to_rgb((hsv_color_t*)&current_hsv));

    while (true){
        LOG_BACKEND_USB_PROCESS();
        NRF_LOG_PROCESS();
    }
}