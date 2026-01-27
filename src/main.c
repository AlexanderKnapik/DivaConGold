#include "buttons/buttons.h"
#include "common/bsp.h"
#include "common/error.h"
#include "common/usb_common.h"
#include "log/log.h"
#include "menu/display.h"
#include "peripheral/i2c.h"
#include "slider/slider.h"
#include "system/systick.h"
#include "usb/usb.h"

#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/i2c.h>
#include <hardware/structs/pio.h>

#include <pico/stdlib.h>

// #include <stdio.h>

#define LOGGING_MODULE_NAME "main"

const struct display_config display_config = {
    .width = DISPLAY_WIDTH,
    .height = DISPLAY_HEIGHT,
    .i2c =
        {
            .instance = DISPLAY_I2C_INSTANCE,
            .speed_hz = DISPLAY_I2C_SPEED_HZ,
            .sda_pin = DISPLAY_I2C_SDA_Pin,
            .scl_pin = DISPLAY_I2C_SCL_Pin,
            .slew = DISPLAY_I2C_SLEW_RATE,
            .drive = DISPLAY_I2C_DRIVE_STRENGTH,
        },
};

const struct i2c_config slider_config = {
    .instance = SLIDER_I2C_INSTANCE,
    .speed_hz = SLIDER_I2C_SPEED_HZ,
    .scl_pin = SLIDER_I2C_SCL_Pin,
    .sda_pin = SLIDER_I2C_SDA_Pin,
    .slew = SLIDER_I2C_SLEW_RATE,
    .drive = SLIDER_I2C_DRIVE_STRENGTH,
};

int main()
{
    set_sys_clock_khz(120000, true);

    buttons_init();

    slider_handle_t slider = slider_open(&slider_config);
    display_handle_t display = display_open(&display_config);
    usb_handle_t usb = usb_open(USB_MODE_KEYBOARD);

    while (true) {
        uint32_t touched_state = 0;

        const struct buttons *buttons = buttons_update();

        enum error err = slider_read(slider, &touched_state);

        if (err == E_SUCCESS || err == E_HARDWARE) {
            err = display_ioctl_touched(display, touched_state);
        }

        if (err == E_SUCCESS) {
            display_update(display);
        }

        if (err == E_SUCCESS) {
            err = usb_write(usb, buttons);
        }
    }

    return 0;
}
