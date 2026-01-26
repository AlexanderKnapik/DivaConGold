#include "slider.h"

#include "common/bsp.h"
#include "common/error.h"
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

enum error slider_read(slider_const_handle_t slider, uint32_t *touched_state)
{
    uint32_t tmp_touched = 0;

    enum error err = read_touched_state(slider, &tmp_touched);

    if (err == E_SUCCESS) {
        err = slider_leds_ioctl_touched(slider->leds, tmp_touched);
    }

    if (err == E_SUCCESS) {
        err = slider_leds_update(slider->leds);
    }

    if (touched_state) {
        *touched_state = tmp_touched;
    }

    return err;
}
