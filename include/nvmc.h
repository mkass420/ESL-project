#ifndef NVMC_H
#define NVMC_H

#include "hsv.h"

void write_color(hsv_color_t* hsv_color);
bool read_color(hsv_color_t* hsv_color);

#endif