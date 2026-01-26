/*****************************************************************************/
/*                            slider_leds.c types                            */
/*****************************************************************************/
#ifndef SLIDER_LEDS_COMMON_H_
#define SLIDER_LEDS_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

enum slider_leds_pattern {
    SLIDER_LEDS_PATTERN_NONE = 0,
    SLIDER_LEDS_PATTERN_RAINBOW = 1,
};

enum slider_leds_animation {
    SLIDER_LEDS_ANIMATION_STATIC = 0,
    SLIDER_LEDS_ANIMATION_CYCLE = 1,
};

const char *slider_leds_pattern_to_string(enum slider_leds_pattern pattern);
const char *slider_leds_animation_to_string(enum slider_leds_animation animation);

#ifdef __cplusplus
}
#endif

#endif // SLIDER_LEDS_COMMON_H_
