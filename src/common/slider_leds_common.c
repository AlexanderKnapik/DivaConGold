#include "slider_leds_common.h"

#include <stddef.h>

const char *slider_leds_pattern_to_string(enum slider_leds_pattern pattern)
{
    switch (pattern) {
    case SLIDER_LEDS_PATTERN_NONE:
        return "None";
    case SLIDER_LEDS_PATTERN_RAINBOW:
        return "Rainbow";
    }

    return NULL;
}

const char *slider_leds_animation_to_string(enum slider_leds_animation animation)
{
    switch (animation) {
    case SLIDER_LEDS_ANIMATION_STATIC:
        return "Static";
    case SLIDER_LEDS_ANIMATION_CYCLE:
        return "Cycle";
    }

    return NULL;
}
