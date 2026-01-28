#include "slider.h"

#include "common/bsp.h"
#include "common/error.h"
#include "common/slider_common.h"
#include "common/util.h"
#include "peripheral/i2c.h"
#include "slider/mpr121.h"
#include "slider/slider_leds.h"

#include <hardware/pio.h>

#include <stddef.h>
#include <stdint.h>

#define LOGGING_MODULE_NAME "slider"

#define SLIDER_MPR121_ELECTRODE_COUNT (8U)
#define SLIDER_MPR121_COUNT (4U)

struct slider_handle {
    mpr121_handle_t mpr121[4];
    slider_leds_handle_t leds;
    struct slider_state prev_state;
};

const struct slider_leds_config slider_leds_config = {
    .pio = SLIDER_LEDS_PIO_INSTANCE,
    .data_pin = SLIDER_LED_DATA_Pin,
    .pattern = SLIDER_LEDS_PATTERN_RAINBOW,
    .animation = SLIDER_LEDS_ANIMATION_STATIC,
    .brightness = 255,
    .touched_colour = {128, 128, 128},
    .player_colour = {255, 0, 0},
};

struct slider_handle slider_handle = {};

slider_handle_t slider_open(const struct i2c_config *i2c)
{
    const struct mpr121_config mpr121_config = {
        .i2c =
            {
                .instance = i2c->instance,
                .speed_hz = i2c->speed_hz,
                .sda_pin = i2c->sda_pin,
                .scl_pin = i2c->scl_pin,
            },

        .auto_configure = true,
        .electrodes = MPR121_ELE_EN_ELE0_ELE11,
        .eleprox = MPR121_ELEPROX_DISABLED,
        .calibration_lock = MPR121_ECR_CL_ENABLED_ELE0_H,

        .touch_threshold = 8,
        .release_threshold = 4,

        .touch_debounce = 2,
        .release_debounce = 2,

        .electrode_filter =
            {
                .rising =
                    {
                        .max_half_delta = 1,
                        .noise_half_delta = 1,
                        .noise_count_limit = 14,
                        .filter_delay_limit = 0,
                    },
                .falling =
                    {
                        .max_half_delta = 1,
                        .noise_half_delta = 5,
                        .noise_count_limit = 1,
                        .filter_delay_limit = 64,

                    },
                .touched =
                    {
                        .noise_half_delta = 0,
                        .noise_count_limit = 0,
                        .filter_delay_limit = 0,
                    },
            },

        .eleprox_filter = {}, /* Default Values */

        .first_filter_samples = MPR121_FIRST_FILTER_SAMPLES_6,
        .second_filter_samples = MPR121_SECOND_FILTER_SAMPLES_4,
        .charge_discharge_current_uA = 16,
        .charge_discharge_time = MPR121_CDT_500_ns,
        .sample_interval = MPR121_SAMPLE_INTERVAL_1_ms,
    };

    slider_handle_t slider = &slider_handle;

    for (uint8_t idx = 0; idx < SLIDER_MPR121_COUNT; idx++) {
        mpr121_handle_t mpr121 = mpr121_open(&mpr121_config, MPR121_I2C_ADDR_0 + idx);

        /* Check that the handles was opened correctly */
        if (!mpr121) {
            slider_close(slider);

            return NULL;
        }

        slider->mpr121[idx] = mpr121;
    }

    slider->leds = slider_leds_open(&slider_leds_config);

    return slider;
}

void slider_close(slider_handle_t slider)
{
    /* Slider already closed, exit early */
    if (!slider) {
        return;
    }

    for (uint8_t idx = 0; idx < SLIDER_MPR121_COUNT; idx++) {
        mpr121_close(slider->mpr121[idx]);
    }
}

static enum error read_touched_state(slider_const_handle_t slider, uint32_t *touched_state)
{
    enum error err = E_SUCCESS;

    for (uint8_t idx = 0; idx < SLIDER_MPR121_COUNT; idx++) {
        uint16_t tmp = 0;
        err = mpr121_read(slider->mpr121[idx], &tmp);

        if (err != E_SUCCESS) {
            return err;
        }

        /*
         * Only using upper 8 electrodes from the 12 available from each MPR121.
         * Converting to an 8 bit value and truncating the 4 LSBs.
         */
        tmp >>= 4;
        tmp &= 0x00FF; /* Discard the eleprox data */

        /* Make a 32 bit integer from all MPR121 reads */
        if (touched_state) {
            *touched_state <<= UINT8_WIDTH;
            *touched_state |= tmp;
        }
    }

    return err;
}

/* The slider itself is broken up into two halves, being left and right.
 * How the registration of the electrodes to any sort of direction is
 * calculated is that the only left and right-most positions of each half
 * are used, ignoring any touches that may be registered between those two
 * extremes.
 *
 * The left and right halves then are mapped onto the left and right joysticks
 * respectively. This is what allows double swipes/stars to be performed.
 * However it also means that in order to perform these double actions, that
 * they can't be performed using only one half of the slider and that the
 * user's finger positions must be centred.
 */
static void decode_direction(enum slider_direction *dir, uint16_t prev_state, uint16_t curr_state)
{
    if (!dir) {
        return;
    }

    /* No previous movement exists to be able to decode a swipe direction */
    if (prev_state == 0) {
        return;
    }

    /* If the slider is no longer being touched, cancel the swipe */
    if (curr_state == 0) {
        *dir = SLIDER_DIRECTION_NONE;
        return;
    }

    /*
     * Note:
     * - Moving to the left is a left bit shift, so an increasing raw value.
     * - Moving to the right is a right bit shift, so a decreasing raw value.
     */
    const uint16_t prev_lsb = lsb_u16(prev_state);
    const uint16_t prev_msb = msb_u16(prev_state);
    const uint16_t curr_lsb = lsb_u16(curr_state);
    const uint16_t curr_msb = msb_u16(curr_state);

    /* Both of the extremes stayed in the same position, keep the swipe in the
     * same position as previous.
     *
     * TODO: Check if this is arcade accurate. I think controllers like the
     * Divaller worked this way only, but not the official arcade controller.
     */
    if ((curr_msb == prev_msb) && (curr_lsb == prev_lsb)) {
        return;
    }

    /* Both of the extremes moved away from each other, cancel the swipe */
    if ((curr_msb > prev_msb) && (curr_lsb < prev_lsb)) {
        *dir = SLIDER_DIRECTION_NONE;
        return;
    }

    /* Both of the extremes moved closer to each other, cancel the swipe */
    if ((curr_msb < prev_msb) && (curr_lsb > prev_lsb)) {
        *dir = SLIDER_DIRECTION_NONE;
        return;
    }

    /*
     * Either or both of the extremes moved an electrode to the left.
     * Register a swipe to the left.
     */
    if ((curr_msb > prev_msb) || (curr_lsb > prev_lsb)) {
        *dir = SLIDER_DIRECTION_LEFT;
        return;
    }

    /*
     * Either or both of the extremes moved an electrode to the right.
     * Register a swipe to the right.
     */
    if ((curr_msb < prev_msb) || (curr_lsb < prev_lsb)) {
        *dir = SLIDER_DIRECTION_RIGHT;
        return;
    }
}

static enum error update_slider_state(slider_handle_t slider, struct slider_state *state,
                                      uint32_t raw_state)
{
    if (!slider) {
        return E_NULL_POINTER;
    }

    /*
     * TODO: Clean up this previous state stuff that's going on. What I had to do was store the
     * previous direction as part of the handle, as otherwise the state may be cleared in main(),
     * this meant that I wasn't able to keep the state in a previously known state as it may be
     * reset.
     */
    /* Decode the left half of the slider */
    decode_direction(&slider->prev_state.left, (slider->prev_state.raw >> UINT16_WIDTH),
                     (raw_state >> UINT16_WIDTH));

    /* Decode the right half of the slider */
    decode_direction(&slider->prev_state.right, slider->prev_state.raw, raw_state);

    slider->prev_state.raw = raw_state;

    if (state) {
        state->raw = slider->prev_state.raw;
        state->left = slider->prev_state.left;
        state->right = slider->prev_state.right;
    }

    return E_SUCCESS;
}

enum error slider_read(slider_handle_t slider, struct slider_state *state)
{
    if (!slider) {
        return E_NULL_POINTER;
    }

    uint32_t raw_state = 0;

    enum error err = read_touched_state(slider, &raw_state);

    if (err == E_SUCCESS) {
        err = update_slider_state(slider, state, raw_state);
    }

    if (err == E_SUCCESS) {
        err = slider_leds_ioctl_touched(slider->leds, raw_state);
    }

    if (err == E_SUCCESS) {
        err = slider_leds_update(slider->leds);
    }

    return err;
}
