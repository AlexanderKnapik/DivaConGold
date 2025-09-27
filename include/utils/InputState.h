#ifndef UTILS_INPUTSTATE_H_
#define UTILS_INPUTSTATE_H_

#include <cstdint>

namespace Divacon::Utils {

struct InputState {
  public:
    struct DPad {
        bool up, down, left, right;
    };

    struct Buttons {
        bool north, east, south, west;
        bool l1, l2, l3;
        bool r1, r2, r3;
        bool start, select, home;
    };

    struct AnalogStick {
        static constexpr uint8_t CENTER = 0x80;

        uint8_t x{CENTER}, y{CENTER};
    };

    struct InputMessage {
        Buttons buttons;
        uint32_t touches;
    };

    DPad dpad{};
    Buttons buttons{};
    struct {
        AnalogStick left;
        AnalogStick right;
    } sticks{};
    uint32_t touches{0};

    void releaseAll() {
        dpad = {};
        buttons = {};
        sticks = {};
        touches = {0};
    };

    [[nodiscard]] InputMessage getInputMessage() const { return {.buttons = buttons, .touches = touches}; };
};

} // namespace Divacon::Utils

#endif // UTILS_INPUTSTATE_H_