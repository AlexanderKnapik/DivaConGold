#ifndef UTILS_MENU_H_
#define UTILS_MENU_H_

#include "utils/InputState.h"
#include "utils/SettingsStore.h"

#include <map>
#include <memory>
#include <stack>
#include <string>
#include <vector>

namespace Divacon::Utils {

class Menu {
  public:
    enum class Page : uint8_t {
        Main,

        DeviceMode,
        Led,
        InputMirrorToDpad,
        Reset,
        Bootsel,

        LedBrightness,
        LedAnimationSpeed,
        LedIdleMode,
        LedIdleColor,
        LedTouchedMode,
        LedTouchedColor,
        LedEnablePlayerColor,
        LedEnablePdloaderSupport,

        LedIdleColorRed,
        LedIdleColorGreen,
        LedIdleColorBlue,

        LedTouchedColorRed,
        LedTouchedColorGreen,
        LedTouchedColorBlue,

        BootselMsg,
    };

    struct State {
        Page page;
        uint8_t selected_value;
        uint8_t original_value;
    };

    struct Descriptor {
        enum class Type : uint8_t {
            Menu,
            Selection,
            Value,
            Toggle,
            RebootInfo,
        };

        enum class Action : uint8_t {
            None,
            GotoParent,

            GotoPageDeviceMode,
            GotoPageLed,
            GotoPageLedBrightness,
            GotoPageLedAnimationSpeed,
            GotoPageLedIdleMode,
            GotoPageLedIdleColor,
            GotoPageLedTouchedMode,
            GotoPageLedTouchedColor,
            GotoPageLedEnablePlayerColor,
            GotoPageLedEnablePdloaderSupport,
            GotoPageInputMirrorToDpad,
            GotoPageReset,
            GotoPageBootsel,

            GotoPageLedIdleColorRed,
            GotoPageLedIdleColorGreen,
            GotoPageLedIdleColorBlue,

            GotoPageLedTouchedColorRed,
            GotoPageLedTouchedColorGreen,
            GotoPageLedTouchedColorBlue,

            SetUsbMode,

            SetLedBrightness,
            SetLedAnimationSpeed,
            SetLedIdleMode,
            SetLedTouchedMode,
            SetLedEnablePlayerColor,
            SetLedEnablePdloaderSupport,

            SetLedIdleColorRed,
            SetLedIdleColorGreen,
            SetLedIdleColorBlue,

            SetLedTouchedColorRed,
            SetLedTouchedColorGreen,
            SetLedTouchedColorBlue,

            SetInputMirrorToDpad,

            DoReset,
            DoRebootToBootsel,
        };

        Type type;
        std::string name;
        std::vector<std::pair<std::string, Action>> items;
    };

    const static std::map<Page, const Descriptor> descriptors;

  private:
    class Buttons {
      public:
        enum class Id : uint8_t { Left, Right, Confirm, Back };

      private:
        struct State {
            enum class Repeat : uint8_t {
                Idle,
                RepeatDelay,
                Repeat,
                FastRepeat,
            };
            Repeat repeat;
            uint32_t pressed_since;
            uint32_t last_repeat;
            bool pressed;
        };

        std::map<Id, State> m_states;

      public:
        Buttons();

        void update(const InputState &state);
        [[nodiscard]] bool getPressed(Id id) const;
    };

    std::shared_ptr<SettingsStore> m_store;
    Buttons m_buttons;
    bool m_active{false};

    std::stack<State> m_state_stack{{{.page = Page::Main, .selected_value = 0, .original_value = 0}}};

    uint8_t getCurrentValue(Page page);
    void gotoPage(Page page);
    void gotoParent(bool do_restore);

    void performAction(Descriptor::Action action, uint8_t value);

  public:
    Menu(std::shared_ptr<SettingsStore> settings_store);

    void activate();
    void update(const InputState &input_state);
    [[nodiscard]] bool active() const;
    State getState();
};
} // namespace Divacon::Utils

#endif // UTILS_MENU_H_