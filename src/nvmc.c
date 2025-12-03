#include "defines.h"
#include "hsv.h"
#include "nrfx_nvmc.h"
#include <stdint.h>

void write_color(hsv_color_t* hsv_color){
    if(!nrfx_nvmc_word_writable_check(FLASH_SAVE_ADDR, hsv_color->raw_data)){
        nrfx_nvmc_page_erase(FLASH_SAVE_ADDR);
    }
    nrfx_nvmc_word_write(FLASH_SAVE_ADDR, hsv_color->raw_data);
    while(!nrfx_nvmc_write_done_check());
}

bool read_color(hsv_color_t* hsv_color){
    hsv_color_t data = {.raw_data = *(uint32_t*)FLASH_SAVE_ADDR};
    if(data.raw_data != 0xFFFFFFFF && isValidHSV(&data)){
        hsv_color->raw_data = data.raw_data;
        return true;
    }
    return false;
}

