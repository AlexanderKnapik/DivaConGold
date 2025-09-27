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
        struct Mpr121x3 {
            std::array<uint8_t, 3> i2c_addresses;

            uint8_t touch_threshold;
            uint8_t release_threshold;
        };

        struct Mpr121x4 {
            std::array<uint8_t, 4> i2c_addresses;

            uint8_t touch_threshold;
            uint8_t release_threshold;
        };

        struct Cap1188 {
            std::array<uint8_t, 4> i2c_addresses;

            uint8_t threshold;
            ::Cap1188::Sensitivity sensitivity;
        };

        struct Is31se5117a {
            std::array<uint8_t, 2> i2c_addresses;

            uint8_t threshold;
            uint8_t hysteresis;
        };

        uint8_t sda_pin;
        uint8_t scl_pin;
        i2c_inst_t *i2c_block;
        uint i2c_speed_hz;

        std::variant<Mpr121x3, Mpr121x4, Cap1188, Is31se5117a> touch_config;
    };

  private:
    // NOLINTNEXTLINE(cppcoreguidelines-special-member-functions): Class has no members
    class TouchControllerInterface {
      public:
        virtual ~TouchControllerInterface() = default;
        virtual uint32_t read() = 0;
    };

    class TouchControllerMpr121x3 : public TouchControllerInterface {
      private:
        std::array<std::unique_ptr<Mpr121>, 3> m_mpr121;

      public:
        TouchControllerMpr121x3(const Config::Mpr121x3 &config, i2c_inst *i2c);

        uint32_t read() final;
    };

    class TouchControllerMpr121x4 : public TouchControllerInterface {
      private:
        std::array<std::unique_ptr<Mpr121>, 4> m_mpr121;

      public:
        TouchControllerMpr121x4(const Config::Mpr121x4 &config, i2c_inst *i2c);

        uint32_t read() final;
    };

    class TouchControllerCap1188 : public TouchControllerInterface {
      private:
        std::array<std::unique_ptr<Cap1188>, 4> m_cap1188;

      public:
        TouchControllerCap1188(const Config::Cap1188 &config, i2c_inst *i2c);

        uint32_t read() final;
    };

    class TouchControllerIs31se5117a : public TouchControllerInterface {
      private:
        std::array<std::unique_ptr<Is31se5117a>, 2> m_is31se5117a;

      public:
        TouchControllerIs31se5117a(const Config::Is31se5117a &config, i2c_inst *i2c);

        uint32_t read() final;
    };

    Config m_config;
    usb_mode_t m_mode;
    uint32_t m_touched{0};

    std::unique_ptr<TouchControllerInterface> m_touch_controller;

    void read();

    void updateInputStateArcade(Utils::InputState &input_state) const;
    void updateInputStateStick(Utils::InputState &input_state) const;

  public:
    TouchSlider(const Config &config, usb_mode_t mode);

    void updateInputState(Utils::InputState &input_state);
};

} // namespace Divacon::Peripherals

#endif // PERIPHERALS_TOUCHSLIDER_H_