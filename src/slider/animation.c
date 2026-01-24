#include "animation.h"

#include "common/error.h"
#include "common/util.h"
#include "slider/colour.h"
#include "slider/pattern_buffer.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * @brief State for controlling reading from a rainbow pattern circular buffer
 *
 * @param pattern The pattern buffer associated with the animation.
 * @param speed How many steps within the circular buffer to increment by every
 * time the animation is updated/read.
 * @param mode The mode of the animation pattern. This is used mainly as a sort
 * of previous state of the speed value, in that if it is static, then the speed
 * should be considered as 0, but if changed back to the cycle, it should
 * remember what the previous speed value was.
 */
struct animation_handle {
    pattern_buffer_handle_t pattern;
    enum animation_mode mode;
    uint8_t speed;
};

animation_handle_t animation_open(enum animation_mode mode, uint8_t speed,
                                  const struct colour *colours, uint16_t size)
{
    if (!colours) {
        return NULL;
    }

    animation_handle_t animation = malloc(sizeof(*animation));

    if (!animation) {
        return NULL;
    }

    pattern_buffer_handle_t pattern = pattern_buffer_open(colours, size);

    if (!pattern) {
        animation_close(animation);
        return NULL;
    }

    /* TODO: Error checking mode inputs */
    animation->pattern = pattern;
    animation->mode = mode;
    animation->speed = speed;

    return animation;
}

void animation_close(animation_handle_t animation)
{
    if (animation) {
        pattern_buffer_close(animation->pattern);
        free(animation);
    }
}

enum error animation_ioctl_speed(animation_handle_t animation, uint8_t speed)
{
    if (!animation) {
        return E_NULL_POINTER;
    }

    animation->speed = speed;

    return E_SUCCESS;
}

enum error animation_ioctl_mode(animation_handle_t animation, enum animation_mode mode)
{
    if (!animation) {
        return E_NULL_POINTER;
    }

    if (mode != ANIMATION_MODE_STATIC && mode != ANIMATION_MODE_CYCLE) {
        return E_INVALID_INPUT;
    }

    animation->mode = mode;

    return E_SUCCESS;
}

static void update_animation_position(animation_handle_t animation)
{
    if (animation && animation->mode == ANIMATION_MODE_CYCLE) {
        pattern_buffer_increment(animation->pattern, animation->speed);
    }
}

/* TODO: Look at https://github.com/FastLED/FastLED/wiki/FastLED-HSV-Colors */
static void adjust_brightness(struct colour *colours, uint16_t size)
{
    for (uint16_t i = 0; i < size; i++) {
        struct colour *const colour = &colours[i];

        /* Divide by 4 */
        colour->r >>= 2;
        colour->g >>= 2;
        colour->b >>= 2;
    }
}

enum error animation_read(animation_handle_t animation, struct colour *dest, uint16_t size)
{
    if (!animation || !dest) {
        return E_NULL_POINTER;
    }

    const uint16_t bytes_read = pattern_buffer_read(animation->pattern, dest, size);

    /* TODO: Hardcoding a brightness value for now. */
    adjust_brightness(dest, size);
    update_animation_position(animation);

    if (bytes_read != size) {
        return E_IO;
    }

    return E_SUCCESS;
}
