#ifndef NVMC_H
#define NVMC_H

#include "color.h"

void write_color(volatile hsv_color_t* hsv_color);
bool read_color(volatile hsv_color_t* hsv_color);
void write_palette(volatile color_palette_t* palette);
bool read_palette(volatile color_palette_t* palette);

#endif