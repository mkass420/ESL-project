#ifndef COLOR_H
#define COLOR_H
#include "app_util.h"
#include "button.h"
#include "defines.h"
#include "nrfx_common.h"
#include <stdint.h>

typedef struct{
    float r; // 0.0-1.0
    float g; // 0.0-1.0
    float b; // 0.0-1.0
} rgb_color_normalized_t;

typedef struct{
    uint8_t r; // 0-255
    uint8_t g; // 0-255
    uint8_t b; // 0-255
} rgb_color_t;

typedef union{
    struct __attribute__((packed)){
        int16_t hue; // 0-360
        int8_t sat;  // 0-100
        int8_t val;  // 0-100
        char color_name[MAX_COLOR_NAME_SIZE]; // zero-terminated
    };
    uint8_t raw_data[HSV_COLOR_SIZE];
} hsv_color_t;

typedef struct{
    float h; // 0.0-1.0
    float s; // 0.0-1.0 
    float v; // 0.0-1.0
} hsv_color_normalized_t;

typedef union{
    struct __attribute__((packed)){
        hsv_color_t colors[MAX_COLORS];
        uint8_t color_count;
    };
    uint8_t raw_data[PALETTE_SIZE];
} color_palette_t;

bool isValidColorName(char* name);
bool isValidHSV(hsv_color_t* hsv);
bool isValidPalette(color_palette_t* palette);

void cycle_hsv_hue(volatile hsv_color_t* hsv);
void cycle_hsv_sat(volatile hsv_color_t* hsv);
void cycle_hsv_val(volatile hsv_color_t* hsv);

typedef void (*hsv_cycle_handler_t)(volatile hsv_color_t*);
static const hsv_cycle_handler_t hsv_cycle_handlers[] = {
    NULL,
    cycle_hsv_hue,
    cycle_hsv_sat,
    cycle_hsv_val
};

void cycle_hsv(input_mode_t mode, volatile hsv_color_t* hsv);
rgb_color_normalized_t hsv_to_rgb(hsv_color_t* hsv);
hsv_color_t rgb_to_hsv(rgb_color_t rgb);
hsv_color_normalized_t hsv_normalize(hsv_color_t* hsv);
rgb_color_normalized_t rgb_normalize(rgb_color_t rgb);

int palette_find_name(char* name, color_palette_t* palette);
bool add_color_to_palette(hsv_color_t* color, color_palette_t* palette);
bool remove_color_from_palette(char* name, color_palette_t* palette);

void apply_color(volatile hsv_color_t* hsv);

#endif