#include "log_utils.h"
#include "color.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_log_backend_usb.h"

extern volatile hsv_color_t current_hsv;

void logs_init(){
    ret_code_t ret = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(ret);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
    NRF_LOG_INFO("Successfully initialized logs");
}

void log_color_hsv(hsv_color_t* hsv){
    NRF_LOG_INFO("name: %s, hue: %d, sat: %d, val: %d", hsv->color_name, hsv->hue, hsv->sat, hsv->val);
}

void log_color_rgb_normalized(rgb_color_normalized_t rgb_norm){
    NRF_LOG_INFO("red: %d, green: %d, blue: %d", rgb_norm.r * 255, rgb_norm.g * 255, rgb_norm.b * 255);
}

void log_palette(color_palette_t* palette){
    for(uint8_t i = 0; i < palette->color_count; i++){
        log_color_hsv(&palette->colors[i]);
    }
}