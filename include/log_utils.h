#ifndef LOG_UTILS_H
#define LOG_UTILS_H

#include "color.h"

void logs_init();
void log_color_hsv(hsv_color_t* hsv);
void log_color_rgb_normalized(rgb_color_normalized_t rgb_norm);
void log_palette(color_palette_t* palette);

#endif