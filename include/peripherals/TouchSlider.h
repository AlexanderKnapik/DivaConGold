#ifndef PERIPHERALS_TOUCHSLIDER_H_
#define PERIPHERALS_TOUCHSLIDER_H_

#include "utils/InputState.h"

#include "usb/device_driver.h"

#include <cap1188/Cap1188.h>
#include <is31se5117a/Is31se5117a.h>
#include <mpr121/Mpr121.h>

#include "hardware/i2c.h"

#include <array>
#include <cstdint>
#include <memory>
#include <variant>

namespace Divacon::Peripherals {

class TouchSlider {
  public:
    struct Config {
        struct Mpr121x4 {
            std::array<uint8_t, 4> i2c_addresses;

            uint8_t touch_threshold;
            uint8_t release_threshold;
        };

        uint8_t sda_pin;
        uint8_t scl_pin;
        i2c_inst_t *i2c_block;
        uint i2c_speed_hz;

        std::variant<Mpr121x4> touch_config;
    };

  private:
    // NOLINTNEXTLINE(cppcoreguidelines-special-member-functions): Class has no members
    class TouchControllerInterface {
      public:
        virtual ~TouchControllerInterface() = default;
        virtual uint32_t read() = 0;
    };

    class TouchControllerMpr121x4 : public TouchControllerInterface {
      private:
        std::array<std::unique_ptr<Mpr121>, 4> m_mpr121;

      public:
        TouchControllerMpr121x4(const Config::Mpr121x4 &config, i2c_inst *i2c);

        uint32_t read() final;
    };

    struct StickStates {
        struct State {
            uint8_t left_limit{0};
            uint8_t right_limit{UINT8_MAX};
            uint8_t value{Utils::InputState::AnalogStick::CENTER};
        };

        State left;
        State right;
    };

    Config m_config;
    usb_mode_t m_mode;
    uint32_t m_touched{0};
    StickStates m_stick_states;

    std::unique_ptr<TouchControllerInterface> m_touch_controller;
    uint32_t m_last_read_time{0};

    void read();

    void updateInputStateArcade(Utils::InputState &input_state) const;
    void updateInputStateStick(Utils::InputState &input_state);

  public:
    TouchSlider(const Config &config, usb_mode_t mode);

    void updateInputState(Utils::InputState &input_state);
};

} // namespace Divacon::Peripherals

#endif // PERIPHERALS_TOUCHSLIDER_H_