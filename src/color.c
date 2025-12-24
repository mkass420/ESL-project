#include "color.h"
#include "app_util.h"
#include "button.h"
#include "defines.h"
#include "pwm.h"
#include <math.h>
#include <stdint.h>
#include <string.h>


volatile bool isDescending_hsv[MODE_COUNT] = {false, false, true, true}; // Порядок движения для каждого из параметров (0 и 1 как заглушки)

bool isValidColorName(char* name){ // NOTE: "/0" is a valid name
    for(char* ch = name; ch < name + MAX_COLOR_NAME_SIZE; ch++){
        if(*ch == '\0') return true;
    }
    return false; // is not zero-terminated or too large
}

bool isValidHSV(hsv_color_t* hsv){
    return hsv->hue >= 0 && hsv->sat >= 0 && hsv->val >= 0 && hsv->hue <= 360 && hsv->sat <= 100 && hsv->val <= 100 && isValidColorName(hsv->color_name);
}

bool isValidPalette(color_palette_t* palette){
    if(palette->color_count > 10) return false;
    for(size_t i = 0; i < palette->color_count; i++){
        if(!isValidHSV(&palette->colors[i])) return false;
    }
    return true;
}

void cycle_hsv_hue(volatile hsv_color_t* hsv){ // движение по кругу
    hsv->hue = (hsv->hue + 1) % 360;
}

void cycle_hsv_sat(volatile hsv_color_t* hsv){ // движение 0-100-0
    int8_t new_sat = hsv->sat; // чтобы избежать значений вне границ если выполнение прерывается ШИМом
    if(isDescending_hsv[MODE_SATURATION]){
        new_sat -= 1;
        if(new_sat <= 0){
            isDescending_hsv[MODE_SATURATION] = false;
            new_sat = 0;
        }
    }
    else{
        new_sat += 1;
        if(new_sat >= 100){
            isDescending_hsv[MODE_SATURATION] = true;    
            new_sat = 100;
        } 
    }
    hsv->sat = new_sat;
}

void cycle_hsv_val(volatile hsv_color_t* hsv){ // движение 0-100-0
    int8_t new_val = hsv->val; // чтобы избежать значений вне границ если выполнение прерывается ШИМом
    if(isDescending_hsv[MODE_BRIGHTNESS]){
        new_val -= 1;
        if(new_val <= 0){ 
            isDescending_hsv[MODE_BRIGHTNESS] = false;
            new_val = 0;
        }
    }
    else{
        new_val += 1;
        if(new_val >= 100){ 
            isDescending_hsv[MODE_BRIGHTNESS] = true;
            new_val = 100;
        }
    }
    hsv->val = new_val;
}

void cycle_hsv(input_mode_t mode, volatile hsv_color_t *hsv){
    if(mode != MODE_NO_INPUT)
        hsv_cycle_handlers[mode](hsv);
}

hsv_color_normalized_t hsv_normalize(hsv_color_t*  hsv){
    return (hsv_color_normalized_t){hsv->hue / 360.0, hsv->sat / 100.0, hsv->val / 100.0};
}

rgb_color_normalized_t rgb_normalize(rgb_color_t rgb){
    return (rgb_color_normalized_t){rgb.r / 255.0, rgb.g / 255.0, rgb.b / 255.0};
}

hsv_color_t rgb_to_hsv(rgb_color_t rgb){
    rgb_color_normalized_t norm_rgb = rgb_normalize(rgb);
    float r = norm_rgb.r;
    float g = norm_rgb.g;
    float b = norm_rgb.b;
    
    float max_val = fmaxf(r, fmaxf(g, b));
    float min_val = fminf(r, fminf(g, b));
    float delta = max_val - min_val;
    
    float h = 0.0;
    float s = 0.0;
    float v = max_val;
    
    if(delta > 0.0){
        if (max_val == r){
            h = 60.0f * fmodf((g - b) / delta, 6.0f);
        } 
        else if (max_val == g){
            h = 60.0f * (((b - r) / delta) + 2.0f);
        } 
        else if (max_val == b){
            h = 60.0f * (((r - g) / delta) + 4.0f);
        }
        if (h < 0.0f) h += 360.0f;
        s = delta / max_val;
    }
    uint16_t hue = ((uint16_t) h) % 360;
    uint8_t  sat = (uint8_t) (s * 100.0);
    uint8_t  val = (uint8_t) (v * 100.0);
    
    return (hsv_color_t){{hue, sat, val, "\0"}};
}

rgb_color_normalized_t hsv_to_rgb(hsv_color_t* hsv){
    hsv_color_normalized_t norm_hsv = hsv_normalize(hsv);
    float h = norm_hsv.h;
    float s = norm_hsv.s;
    float v = norm_hsv.v;
    
    if(hsv->sat == 0){
        return (rgb_color_normalized_t){v, v, v};
    }
    
    int sector = (int)(h * 6.0);
    float f = h * 6.0 - sector;
    float p = v * (1.0 - s);
    float q = v * (1.0 - s * f);
    float t = v * (1.0 - s * (1.0 - f));
    
    switch (sector){
        case 0:  return (rgb_color_normalized_t){v, t, p};
        case 1:  return (rgb_color_normalized_t){q, v, p};
        case 2:  return (rgb_color_normalized_t){p, v, t};
        case 3:  return (rgb_color_normalized_t){p, q, v};
        case 4:  return (rgb_color_normalized_t){t, p, v};
        case 5:  return (rgb_color_normalized_t){v, p, q};
        default: return (rgb_color_normalized_t){1.0, 1.0, 1.0};
    }
}

int palette_find_name(char* name, color_palette_t* palette){
    if(!isValidColorName(name)) return -1;
    for(size_t i = 0; i < palette->color_count; i++){
        if(strcmp(palette->colors[i].color_name, name) == 0) return i;
    }
    return -1;
}

bool add_color_to_palette(hsv_color_t* color, color_palette_t* palette){
    if(!isValidHSV(color)) return false;
    if(palette->color_count == 10) return false;
    palette->colors[palette->color_count] = *color;
    palette->color_count += 1;
    return true;
}

bool remove_color_from_palette(char* name, color_palette_t* palette){
    int pos = palette_find_name(name, palette);
    if(pos == -1) return false;
    for(int i = pos + 1; i < palette->color_count; i++){
        palette->colors[i - 1] = palette->colors[i]; 
    }
    memset(palette->colors[palette->color_count - 1].raw_data, 0, HSV_COLOR_SIZE);
    palette->color_count -= 1;
    return true;
}

void apply_color(volatile hsv_color_t* hsv){
    rgb_color_normalized_t rgb = hsv_to_rgb((hsv_color_t*)hsv);
    pwm_rgb_led_set_color(rgb);
}