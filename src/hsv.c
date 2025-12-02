#include "hsv.h"
#include "button.h"
#include <stdint.h>


volatile bool isDescending_hsv[MODE_COUNT] = {false, false, true, true}; // Порядок движения для каждого из параметров (0 и 1 как заглушки)

void cycle_hsv_hue(hsv_color_t* hsv){ // движение по кругу
    hsv->hue = (hsv->hue + 1) % 360;
}

void cycle_hsv_sat(hsv_color_t* hsv){ // движение 0-100-0
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

void cycle_hsv_val(hsv_color_t* hsv){ // движение 0-100-0
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

void cycle_hsv(input_mode_t mode, hsv_color_t *hsv){
    if(mode != MODE_NO_INPUT)
        hsv_cycle_handlers[mode](hsv);
}

hsv_color_normalized_t hsv_normalize(hsv_color_t hsv){
    return (hsv_color_normalized_t){hsv.hue / 360.0, hsv.sat / 100.0, hsv.val / 100.0};
}

rgb_color_normalized_t hsv_to_rgb(hsv_color_t hsv){
    hsv_color_normalized_t norm_hsv = hsv_normalize(hsv);
    float h = norm_hsv.h;
    float s = norm_hsv.s;
    float v = norm_hsv.v;
    
    if(hsv.sat == 0){
        return (rgb_color_normalized_t){v, v, v};
    }
    
    int sector = (int)(h * 6.0);
    float f = h * 6.0 - sector;
    float p = v * (1.0 - s);
    float q = v * (1.0 - s * f);
    float t = v * (1.0 - s * (1.0 - f));
    
    switch (sector){
        case 0: return (rgb_color_normalized_t){v, t, p};
        case 1: return (rgb_color_normalized_t){q, v, p};
        case 2: return (rgb_color_normalized_t){p, v, t};
        case 3: return (rgb_color_normalized_t){p, q, v};
        case 4: return (rgb_color_normalized_t){t, p, v};
        case 5: return (rgb_color_normalized_t){v, p, q};
        default: return (rgb_color_normalized_t){1.0, 1.0, 1.0};
    }
}