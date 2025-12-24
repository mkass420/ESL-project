#include "nvmc.h"
#include "defines.h"
#include "color.h"
#include "nrfx_nvmc.h"
#include <stdint.h>
#include <string.h>

void write_color(volatile hsv_color_t* hsv_color){
    color_palette_t current_palette = {0};
    uint8_t empty_palette[PALETTE_SIZE] = {0};
    if(!nrfx_nvmc_word_writable_check(FLASH_SAVE_CURRENT_ADDR, hsv_color->raw_data[0])){
        nrfx_nvmc_page_erase(FLASH_PAGE_ADDR);
        read_palette(&current_palette);
    }
    nrfx_nvmc_words_write(FLASH_SAVE_CURRENT_ADDR, (uint8_t*)hsv_color->raw_data, HSV_COLOR_SIZE);
    while(!nrfx_nvmc_write_done_check());
    if(memcmp(current_palette.raw_data, empty_palette, PALETTE_SIZE) != 0){
        nrfx_nvmc_words_write(FLASH_SAVE_CURRENT_ADDR, current_palette.raw_data, PALETTE_SIZE);
        while(!nrfx_nvmc_write_done_check());
    }
}

bool read_color(volatile hsv_color_t* hsv_color){
    hsv_color_t data = {0};
    uint8_t invalid[HSV_COLOR_SIZE];
    memset(invalid, UINT8_MAX, HSV_COLOR_SIZE);
    memcpy(data.raw_data, (uint8_t*)FLASH_SAVE_CURRENT_ADDR, HSV_COLOR_SIZE);
    if(memcmp(data.raw_data, invalid, HSV_COLOR_SIZE) != 0 && isValidHSV(&data)){
        memcpy((uint8_t*)hsv_color->raw_data, data.raw_data, HSV_COLOR_SIZE);
        return true;
    }
    return false;
}

void write_palette(volatile color_palette_t* palette){
    hsv_color_t current_color = {0};
    uint8_t empty_color[HSV_COLOR_SIZE] = {0};
    if(!nrfx_nvmc_word_writable_check(FLASH_SAVE_PALETTE_ADDR, palette->raw_data[0])){
        nrfx_nvmc_page_erase(FLASH_PAGE_ADDR);
        read_color(&current_color);
    }
    nrfx_nvmc_words_write(FLASH_SAVE_PALETTE_ADDR, (uint8_t*)palette->raw_data, PALETTE_SIZE);
    while(!nrfx_nvmc_write_done_check());
    if(memcmp(current_color.raw_data, empty_color, HSV_COLOR_SIZE) != 0){
        nrfx_nvmc_words_write(FLASH_SAVE_CURRENT_ADDR, current_color.raw_data, HSV_COLOR_SIZE);
        while(!nrfx_nvmc_write_done_check());
    }
}

bool read_palette(volatile color_palette_t* palette){
    color_palette_t data = {0};
    uint8_t invalid[PALETTE_SIZE];
    memset(invalid, UINT8_MAX, PALETTE_SIZE);
    memcpy(data.raw_data, (uint8_t*)FLASH_SAVE_PALETTE_ADDR, PALETTE_SIZE);
    if(memcmp(data.raw_data, invalid, PALETTE_SIZE) != 0 && isValidPalette(&data)){
        memcpy((uint8_t*)palette->raw_data, data.raw_data, PALETTE_SIZE);
        return true;
    }
    return false;
}

