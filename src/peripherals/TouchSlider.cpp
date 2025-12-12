#include "peripherals/TouchSlider.h"

#include "hardware/gpio.h"

namespace Divacon::Peripherals {

TouchSlider::TouchControllerMpr121x4::TouchControllerMpr121x4(const TouchSlider::Config::Mpr121x4 &config,
                                                              i2c_inst *i2c) {
    size_t idx = 0;
    for (auto &mpr121 : m_mpr121) {
        mpr121 = std::make_unique<Mpr121>(config.i2c_addresses.at(idx), i2c, config.touch_threshold,
                                          config.release_threshold, true);
        idx++;
    }
}

uint32_t TouchSlider::TouchControllerMpr121x4::read() {
    // Electrodes are mapped according to below table.
    //
    //         | m_mpr121[0] | m_mpr121[1] | m_mpr121[2] | m_mpr121[3] |
    // --------+-------------+-------------+-------------+-------------+
    // Pin     |    4..11    |    4..11    |    4..11    |    4..11    |
    // Touched |   31..24    |   23..16    |   15..8     |    7..0     |

    return ((m_mpr121[0]->getTouched() & 0x0FF0) << 20) | ((m_mpr121[1]->getTouched() & 0x0FF0) << 12) |
           ((m_mpr121[2]->getTouched() & 0x0FF0) << 4) | ((m_mpr121[3]->getTouched() & 0x0FF0) >> 4);
}

TouchSlider::TouchSlider(const Config &config, usb_mode_t mode) : m_config(config), m_mode(mode) {
    gpio_set_function(m_config.sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(m_config.scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(m_config.sda_pin);
    gpio_pull_up(m_config.scl_pin);

    i2c_init(m_config.i2c_block, m_config.i2c_speed_hz);

    std::visit(
        [this](auto &&config) {
            m_touch_controller = std::make_unique<TouchControllerMpr121x4>(config, m_config.i2c_block);
        },
        m_config.touch_config);
}

void TouchSlider::updateInputStateArcade(Utils::InputState &input_state) const {
    // The 32bit state vector is mapped into the 4 8bit axes of the analog sticks, XORed
    // with the stick center postion to ensure no stick movement when the slider is not touched.
    input_state.sticks.right.y = (uint8_t)((m_touched & 0xFF000000) >> 24) ^ Utils::InputState::AnalogStick::CENTER;
    input_state.sticks.right.x = (uint8_t)((m_touched & 0x00FF0000) >> 16) ^ Utils::InputState::AnalogStick::CENTER;
    input_state.sticks.left.y = (uint8_t)((m_touched & 0x0000FF00) >> 8) ^ Utils::InputState::AnalogStick::CENTER;
    input_state.sticks.left.x = (uint8_t)((m_touched & 0x000000FF)) ^ Utils::InputState::AnalogStick::CENTER;
}

void TouchSlider::updateInputStateStick(Utils::InputState &input_state) {
    auto handleSide = [](uint16_t touched, StickStates::State &previous_state, uint8_t &target) {
        if (touched != 0) {
            uint8_t left_limit = 0;
            uint8_t right_limit = UINT8_MAX;

            // Find leftmost touch
            for (uint8_t i = 0; i < 16; ++i) {
                if ((0x0001 << (15 - i)) & touched) {
                    left_limit = (15 - i);
                    break;
                }
            }
            // Find rightmost touch
            for (uint8_t i = 0; i < 16; ++i) {
                if ((0x0001 << i) & touched) {
                    right_limit = i;
                    break;
                }
            }

            // Either of the extreme positions moved left
            if (((left_limit > previous_state.left_limit) && (right_limit >= previous_state.right_limit)) ||
                ((left_limit >= previous_state.left_limit) && (right_limit > previous_state.right_limit))) {
                target = 0;
                // Either of the extreme positions moved right
            } else if (((left_limit < previous_state.left_limit) && (right_limit <= previous_state.right_limit)) ||
                       ((left_limit <= previous_state.left_limit) && (right_limit < previous_state.right_limit))) {
                target = UINT8_MAX;
                // No movement, but still touched
            } else {
                target = previous_state.value;
            }

            previous_state.left_limit = left_limit;
            previous_state.right_limit = right_limit;
        } else {
            // No touch, reset
            previous_state.left_limit = 0;
            previous_state.right_limit = UINT8_MAX;
            target = Utils::InputState::AnalogStick::CENTER;
        }
        previous_state.value = target;
    };

    // Interpret slider as two distinctive zones, controlling left and right
    // stick x-axis respectively
    handleSide(m_touched >> 16, m_stick_states.left, input_state.sticks.left.x);
    handleSide(m_touched & 0x0000FFFF, m_stick_states.right, input_state.sticks.right.x);

    input_state.sticks.left.y = Utils::InputState::AnalogStick::CENTER;
    input_state.sticks.right.y = Utils::InputState::AnalogStick::CENTER;
}

void TouchSlider::updateInputState(Utils::InputState &input_state) {

    read();

    switch (m_mode) {

    case USB_MODE_SWITCH_DIVACON:
    case USB_MODE_PS4_DIVACON:
    case USB_MODE_PDLOADER:
    case USB_MODE_MIDI:
    case USB_MODE_DEBUG:
        updateInputStateArcade(input_state);
        break;
    case USB_MODE_SWITCH_HORIPAD:
    case USB_MODE_DUALSHOCK3:
    case USB_MODE_DUALSHOCK4:
    case USB_MODE_PS4_COMPAT:
    case USB_MODE_XBOX360:
    case USB_MODE_KEYBOARD:
        updateInputStateStick(input_state);
        break;
    }

    input_state.touches = m_touched;
}

void TouchSlider::read() {
    const uint32_t now = to_ms_since_boot(get_absolute_time());
    if ((m_last_read_time + 1) <= now) {
        m_touched = m_touch_controller->read();
    }

    m_last_read_time = now;
}

} // namespace Divacon::Peripherals