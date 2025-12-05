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
                    .up = 11,
                    .down = 12,
                    .left = 4,
                    .right = 3,
                },
            .buttons =
                {
                    .north = 16,
                    .east = 13,
                    .south = 14,
                    .west = 15,

                    .l1 = 10,
                    .l2 = 9,
                    .l3 = 8,

                    .r1 = 2,
                    .r2 = 1,
                    .r3 = 0,

                    .start = 7,
                    .select = 5,
                    .home = 6,
                },
        },
    .mirror_to_dpad = false,
    .debounce_delay_ms = 3,
};

const Peripherals::ButtonLeds::Config button_leds_config = {
    .pins =
        {
            .north = 28,
            .east = 22,
            .south = 26,
            .west = 27,
        },
    .invert = false,
};

const Peripherals::TouchSlider::Config touch_slider_config = {
    .sda_pin = 18,
    .scl_pin = 19,
    .i2c_block = i2c1,
    .i2c_speed_hz = 400000,

    .touch_config =
        Peripherals::TouchSlider::Config::Mpr121x4{
            .i2c_addresses = {0x5A, 0x5B, 0x5C, 0x5D},
            .touch_threshold = 12,
            .release_threshold = 6,
        },
};

const Peripherals::TouchSliderLeds::Config touch_slider_leds_config = {
    .led_pin = 17,
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
    .sda_pin = 20,
    .scl_pin = 21,
    .i2c_block = i2c0,
    .i2c_speed_hz = 400000,
    .i2c_address = 0x3C,
};

} // namespace Divacon::Config::Default

#endif // GLOBALCONFIGURATION_H_
