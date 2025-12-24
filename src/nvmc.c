#include "nvmc.h"
#include "defines.h"
#include "color.h"
#include "nrfx_nvmc.h"
#include <stdint.h>
#include <string.h>

static bool can_write_without_erase(uint32_t address, const uint32_t* src, size_t size_bytes) {
    uint32_t* flash_ptr = (uint32_t*)address;
    size_t words = size_bytes / 4;
    
    for(size_t i = 0; i < words; i++) {
        uint32_t current_val = flash_ptr[i];
        uint32_t new_val = src[i];
        
        if (current_val == new_val) continue;
        if ((current_val & new_val) != new_val) {
            return false; // Requires turning a 0 into a 1 -> Erase needed
        }
    }
    return true;
}

void write_color(volatile hsv_color_t* hsv_color){
    color_palette_t saved_palette = {0};
    bool palette_valid = false;

    if(!can_write_without_erase(FLASH_SAVE_CURRENT_ADDR, (uint32_t*)hsv_color->raw_data, HSV_COLOR_SIZE)){
        palette_valid = read_palette(&saved_palette);
        nrfx_nvmc_page_erase(FLASH_PAGE_ADDR);
        
        if(palette_valid){
             nrfx_nvmc_words_write(FLASH_SAVE_PALETTE_ADDR, (const uint32_t*)saved_palette.raw_words, PALETTE_WORDS);
             while(!nrfx_nvmc_write_done_check());
        }
    }

    nrfx_nvmc_words_write(FLASH_SAVE_CURRENT_ADDR, (const uint32_t*)hsv_color->raw_data,HSV_COLOR_WORDS);
    while(!nrfx_nvmc_write_done_check());
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
    hsv_color_t saved_color = {0};
    bool color_valid = false;

    if(!can_write_without_erase(FLASH_SAVE_PALETTE_ADDR, (uint32_t*)palette->raw_data, PALETTE_SIZE)){
        color_valid = read_color(&saved_color);
        nrfx_nvmc_page_erase(FLASH_PAGE_ADDR);
        if(color_valid){
            nrfx_nvmc_words_write(FLASH_SAVE_CURRENT_ADDR, (const uint32_t*)saved_color.raw_words, HSV_COLOR_WORDS);
            while(!nrfx_nvmc_write_done_check());
        }
    }

    nrfx_nvmc_words_write(FLASH_SAVE_PALETTE_ADDR, (const uint32_t*)palette->raw_words,PALETTE_WORDS);
    while(!nrfx_nvmc_write_done_check());
}

bool read_palette(volatile color_palette_t* palette){
    color_palette_t data = {0};
    uint8_t invalid[PALETTE_SIZE];
    memset(invalid, 0xFF, PALETTE_SIZE);
    memcpy(data.raw_data, (uint8_t*)FLASH_SAVE_PALETTE_ADDR, PALETTE_SIZE);
    if(memcmp(data.raw_data, invalid, PALETTE_SIZE) != 0 && isValidPalette(&data)){
        memcpy((uint8_t*)palette->raw_data, data.raw_data, PALETTE_SIZE);
        return true;
    }
    return false;
}

