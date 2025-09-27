#ifndef PERIPHERALS_BUTTONS_H_
#define PERIPHERALS_BUTTONS_H_

#include "utils/InputState.h"

#include <cstdint>
#include <map>

namespace Divacon::Peripherals {

class Buttons {
  public:
    struct Config {
        struct {
            struct {
                uint8_t up;
                uint8_t down;
                uint8_t left;
                uint8_t right;
            } dpad;

            struct {
                uint8_t north;
                uint8_t east;
                uint8_t south;
                uint8_t west;

                uint8_t l1;
                uint8_t l2;
                uint8_t l3;

                uint8_t r1;
                uint8_t r2;
                uint8_t r3;

                uint8_t start;
                uint8_t select;
                uint8_t home;
            } buttons;
        } pins;

        bool mirror_to_dpad;
        uint8_t debounce_delay_ms;
    };

  private:
    enum class Id {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        NORTH,
        EAST,
        SOUTH,
        WEST,
        L1,
        L2,
        L3,
        R1,
        R2,
        R3,
        START,
        SELECT,
        HOME,
    };

    class Button {
      private:
        uint8_t m_gpio_pin;
        uint32_t m_gpio_mask;

        uint32_t m_last_change{0};
        bool m_active{false};

      public:
        Button(uint8_t pin);

        [[nodiscard]] uint8_t getGpioPin() const { return m_gpio_pin; };
        [[nodiscard]] uint32_t getGpioMask() const { return m_gpio_mask; };

        [[nodiscard]] bool getState() const { return m_active; };
        void setState(bool state, uint8_t debounce_delay);
    };

    struct SocdState {
        Id lastVertical;
        Id lastHorizontal;
    };

    Config m_config;
    SocdState m_socd_state{.lastVertical = Id::DOWN, .lastHorizontal = Id::RIGHT};
    std::map<Id, Button> m_buttons;

    void socdClean(Utils::InputState &input_state);

  public:
    Buttons(const Config &config);

    void setMirrorToDpad(bool mirror_to_dpad);

    void updateInputState(Utils::InputState &input_state);
};

} // namespace Divacon::Peripherals

#endif // PERIPHERALS_BUTTONS_H_