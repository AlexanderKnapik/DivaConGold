#ifndef GLOBALCONFIGURATION_H_
#define GLOBALCONFIGURATION_H_

#include "peripherals/ButtonLeds.h"
#include "peripherals/Buttons.h"
#include "peripherals/Display.h"
#include "peripherals/TouchSlider.h"
#include "peripherals/TouchSliderLeds.h"
#include "usb/device_driver.h"

#include "hardware/i2c.h"

namespace Divacon::Config::Default {

const usb_mode_t usb_mode = USB_MODE_SWITCH_DIVACON;

const Peripherals::Buttons::Config buttons_config = {
    .pins =
        {
            .dpad =
                {
                    .up = 18,
                    .down = 19,
                    .left = 20,
                    .right = 21,
                },
            .buttons =
                {
                    .north = 6,
                    .east = 9,
                    .south = 8,
                    .west = 7,

                    .l1 = 0,
                    .l2 = 1,
                    .l3 = 2,

                    .r1 = 3,
                    .r2 = 4,
                    .r3 = 5,

                    .start = 26,
                    .select = 27,
                    .home = 22,
                },
        },
    .mirror_to_dpad = false,
    .debounce_delay_ms = 3,
};

const Peripherals::ButtonLeds::Config button_leds_config = {
    .pins =
        {
            .north = 10,
            .east = 13,
            .south = 12,
            .west = 11,
        },
    .invert = false,
};

const Peripherals::TouchSlider::Config touch_slider_config = {
    .sda_pin = 16,
    .scl_pin = 17,
    .i2c_block = i2c0,
    .i2c_speed_hz = 800000,

    //
    // Touch controller config, either Mpr121x3, Mpr121x4 or Cap1188
    //

    // .touch_config =
    //     Peripherals::TouchSlider::Config::Mpr121x3{
    //         .i2c_addresses = {0x5A, 0x5D, 0x5C},
    //         .touch_threshold = 12,
    //         .release_threshold = 6,
    //     },

    .touch_config =
        Peripherals::TouchSlider::Config::Mpr121x4{
            .i2c_addresses = {0x5A, 0x5B, 0x5C, 0x5D},
            .touch_threshold = 12,
            .release_threshold = 6,
        },

    // .touch_config =
    //     Peripherals::TouchSlider::Config::Cap1188{
    //         .i2c_addresses = {0x2C, 0x2B, 0x2A, 0x29},
    //         .threshold = 64,
    //         .sensitivity = Cap1188::Sensitivity::S32,
    //     },
};

const Peripherals::TouchSliderLeds::Config touch_slider_leds_config = {
    .led_pin = 28,
    .is_rgbw = false,
    .reverse = true,
    .leds_per_segment = 2,

    .brightness = 128,
    .animation_speed = 128,
    .idle_mode = Peripherals::TouchSliderLeds::Config::IdleMode::Pulse,
    .touched_mode = Peripherals::TouchSliderLeds::Config::TouchedMode::TouchedFade,
    .idle_color = {.r = 64, .g = 64, .b = 64},
    .touched_color = {.r = 138, .g = 254, .b = 171},
    .enable_player_color = true,
    .enable_pdloader_support = true,
};

const Peripherals::Display::Config display_config = {
    .sda_pin = 14,
    .scl_pin = 15,
    .i2c_block = i2c1,
    .i2c_speed_hz = 1000000,
    .i2c_address = 0x3C,
};

} // namespace Divacon::Config::Default

#endif // GLOBALCONFIGURATION_H_