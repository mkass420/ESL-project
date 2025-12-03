#ifndef HSV_H
#define HSV_H
#include "app_util.h"
#include "button.h"
#include "nrfx_common.h"
#include <stdint.h>

typedef struct{
    float r; // 0.0-1.0
    float g; // 0.0-1.0
    float b; // 0.0-1.0
} rgb_color_normalized_t;

typedef union{
    struct __attribute__((packed)){
        int16_t hue; // 0-360
        int8_t sat;  // 0-100
        int8_t val;  // 0-100
    };
    uint32_t raw_data;
} hsv_color_t; // 4 bytes
STATIC_ASSERT(sizeof(hsv_color_t) == 4, "hsv_color_t should be one 32-bit word");

typedef struct{
    float h; // 0.0-1.0
    float s; // 0.0-1.0 
    float v; // 0.0-1.0
} hsv_color_normalized_t;

bool isValidHSV(hsv_color_t* hsv);

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