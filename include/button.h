#ifndef BUTTON_H
#define BUTTON_H

#define FOREACH_MODE(MODE) \
    MODE(MODE_NO_INPUT) \
    MODE(MODE_HUE) \
    MODE(MODE_SATURATION) \
    MODE(MODE_BRIGHTNESS) \
    MODE(MODE_COUNT) \

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

typedef enum{
    FOREACH_MODE(GENERATE_ENUM)
} input_mode_t;

__attribute__((unused)) static const char *MODE_STRING[] = {
    FOREACH_MODE(GENERATE_STRING)
};

// typedef enum{
//     MODE_NO_INPUT,
//     MODE_HUE,
//     MODE_SATURATION,
//     MODE_BRIGHTNESS,
//     MODE_COUNT
// } input_mode_t;

typedef enum {
    BUTTON_STATE_RELEASED,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_HOLD
} button_state_t;

void timers_init(void);

void button_init(void);

#endif