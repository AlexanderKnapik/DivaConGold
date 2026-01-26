#include "slider_leds.h"

#include "common/bsp.h"
#include "common/error.h"
#include "common/util.h"
#include "slider/animation.h"
#include "slider/colour.h"

#include "pio_ws2812/ws2812.h"

#include <hardware/gpio.h>
#include <hardware/structs/pio.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define LED_IS_RGBW (false)
#define ANIMATION_SPEED (1U)

enum slider_leds_touched_mode {
    SLIDER_LEDS_TOUCHED_MODE_OFF = 0,
    SLIDER_LEDS_TOUCHED_MODE_IDLE,
    SLIDER_LEDS_TOUCHED_MODE_TOUCHED,
    SLIDER_LEDS_TOUCHED_MODE_TOUCHED_FADE,
    SLIDER_LEDS_TOUCHED_MODE_TOUCHED_IDLE,
};

struct slider_leds_handle {
    pio_hw_t *pio;
    animation_handle_t animation;

    uint8_t brightness;
    struct colour touched_colour;
    struct colour player_colour;
    uint32_t touched_state;
};

typedef struct slider_leds_handle *slider_leds_handle_t;
static struct slider_leds_handle slider_leds_handle = {};

static void reset_leds(pio_hw_t *pio)
{
    uint32_t frame[SLIDER_LEDS_COUNT] = {};
    ws2812_put_frame(pio, frame, ARRAY_SIZE(frame));
}

slider_leds_handle_t slider_leds_open(const struct slider_leds_config *config)
{
    slider_leds_handle_t leds = &slider_leds_handle;

    *leds = (struct slider_leds_handle){
        .pio = config->pio,
        .brightness = config->brightness,
        .touched_colour = {config->touched_colour.r, config->touched_colour.g,
                           config->touched_colour.b},
        .player_colour = {config->player_colour.r, config->player_colour.g,
                          config->player_colour.b},
        .touched_state = 0,
        .animation = NULL,
    };

    gpio_pull_up(config->data_pin); /* Redundancy sake */
    gpio_set_slew_rate(config->data_pin, config->slew);
    gpio_set_drive_strength(config->data_pin, config->drive);
    ws2812_init(config->pio, config->data_pin, LED_IS_RGBW);

    /* Clear the LEDs on a microcontroller reset */
    reset_leds(config->pio);

    if (config->pattern == SLIDER_LEDS_PATTERN_RAINBOW) {
        leds->animation = animation_open((enum animation_mode)config->animation, ANIMATION_SPEED,
                                         rainbow_colours, ARRAY_SIZE(rainbow_colours));
    }

    if (animation_ioctl_speed(slider_leds_handle.animation, config->animation) != E_SUCCESS) {
        return NULL;
    }

    return leds;
}

void slider_leds_close(slider_leds_handle_t leds)
{
    /* Don't free leds as it is a statically allocated singleton */
    if (leds) {
        reset_leds(leds->pio);
        animation_close(leds->animation);
    }
}

static bool is_electrode_active(uint32_t touched_state, uint8_t electrode)
{
    /* If the electrode within the touched bitmask is currently active */
    return (bool)(touched_state & (1 << electrode));
}

static uint32_t colour_to_u32pixel(const struct colour *colour)
{
    return ws2812_rgb_to_gamma_corrected_u32pixel(colour->r, colour->g, colour->b);
}

enum error slider_leds_update(slider_leds_handle_t leds)
{
    if (!leds) {
        return E_NULL_POINTER;
    }

    /* Copy from the animation to a temp buffer */
    struct colour colours[SLIDER_LEDS_COUNT];
    uint32_t frame[SLIDER_LEDS_COUNT] = {};
    animation_read(leds->animation, colours, ARRAY_SIZE(frame));

    /* TODO: Make the two loops a one pass step */
    /* Load the colours into the frame buffer */
    for (uint8_t i = 0; i < 64; i++) {
        frame[i] = colour_to_u32pixel(&colours[i]);
    }

    /* Overwrite the sections that are touched with the touched_colour */
    for (uint8_t electrode = 0; electrode < SLIDER_ELECTRODE_COUNT; electrode++) {
        if (is_electrode_active(leds->touched_state, electrode)) {
            const uint32_t pixel = colour_to_u32pixel(&leds->touched_colour);

            /* 2 LEDs per electrode, so multiply the index by 2 */
            /* Because the LED string starts on the left-hand side of the
            slider and the WS2812s work by shifting out the */
            const uint8_t idx = (SLIDER_LEDS_COUNT - 1) - (electrode << 1);
            frame[idx] = pixel;
            frame[idx - 1] = pixel;
        }
    }

    ws2812_put_frame(leds->pio, frame, SLIDER_LEDS_COUNT);

    return E_SUCCESS;
}

enum error slider_leds_ioctl_touched(slider_leds_handle_t leds, uint32_t touched_state)
{
    if (!leds) {
        return E_NULL_POINTER;
    }

    leds->touched_state = touched_state;

    return E_SUCCESS;
}
