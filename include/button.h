#ifndef BUTTON_H
#define BUTTON_H

typedef enum{
    MODE_NO_INPUT,
    MODE_HUE,
    MODE_SATURATION,
    MODE_BRIGHTNESS,
    MODE_COUNT
} input_mode_t;

typedef enum {
    BUTTON_STATE_RELEASED,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_HOLD
} button_state_t;

void timers_init(void);

void button_init(void);

#endif