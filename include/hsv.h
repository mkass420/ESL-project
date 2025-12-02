#ifndef HSV_H
#define HSV_H
#include "button.h"
#include "nrfx_common.h"
#include <stdint.h>

typedef struct{
    float r; // 0.0-1.0
    float g; // 0.0-1.0
    float b; // 0.0-1.0
} rgb_color_normalized_t;

typedef struct{
    int16_t hue; // 0-360
    int8_t sat;  // 0-100
    int8_t val;  // 0-100
} hsv_color_t;

typedef struct{
    float h; // 0.0-1.0
    float s; // 0.0-1.0 
    float v; // 0.0-1.0
} hsv_color_normalized_t;

void cycle_hsv_hue(hsv_color_t* hsv);
void cycle_hsv_sat(hsv_color_t* hsv);
void cycle_hsv_val(hsv_color_t* hsv);

typedef void (*hsv_cycle_handler_t)(hsv_color_t*);
static const hsv_cycle_handler_t hsv_cycle_handlers[] = {
    NULL,
    cycle_hsv_hue,
    cycle_hsv_sat,
    cycle_hsv_val
};

void cycle_hsv(input_mode_t mode, hsv_color_t* hsv);
rgb_color_normalized_t hsv_to_rgb(hsv_color_t hsv);
hsv_color_normalized_t hsv_normalize(hsv_color_t hsv);


#endif