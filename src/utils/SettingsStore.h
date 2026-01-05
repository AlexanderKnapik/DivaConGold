#ifndef UTILS_SETTINGSSTORE_H_
#define UTILS_SETTINGSSTORE_H_

#include "peripherals/TouchSliderLeds.h"
#include "usb/device_driver.h"

#include "hardware/flash.h"

namespace Divacon::Utils {

class SettingsStore {
  private:
    const static uint32_t m_flash_size = FLASH_SECTOR_SIZE;
    const static uint32_t m_flash_offset = PICO_FLASH_SIZE_BYTES - m_flash_size;
    const static uint32_t m_store_size = FLASH_PAGE_SIZE;
    const static uint32_t m_store_pages = m_flash_size / m_store_size;
    const static uint8_t m_magic_byte = 0x39;

    struct __attribute((packed, aligned(1))) Storecache {
        uint8_t in_use;
        usb_mode_t usb_mode;
        uint8_t led_brightness;
        uint8_t led_animation_speed;
        Peripherals::TouchSliderLeds::Config::IdleMode led_idle_mode;
        Peripherals::TouchSliderLeds::Config::TouchedMode led_touched_mode;
        Peripherals::TouchSliderLeds::Config::Color led_idle_color;
        Peripherals::TouchSliderLeds::Config::Color led_touched_color;
        bool led_enable_player_color;
        bool led_enable_pdloader_support;
        bool buttons_mirror_to_dpad;

        std::array<uint8_t, m_store_size - sizeof(uint8_t) - sizeof(usb_mode_t) - sizeof(uint8_t) - sizeof(uint8_t) -
                                sizeof(Peripherals::TouchSliderLeds::Config::IdleMode) -
                                sizeof(Peripherals::TouchSliderLeds::Config::TouchedMode) -
                                sizeof(Peripherals::TouchSliderLeds::Config::Color) -
                                sizeof(Peripherals::TouchSliderLeds::Config::Color) - sizeof(bool) - sizeof(bool) -
                                sizeof(bool)>
            _padding;
    };
    static_assert(sizeof(Storecache) == m_store_size);

    enum class RebootType : uint8_t {
        None,
        Normal,
        Bootsel,
    };

    Storecache m_store_cache;
    bool m_dirty{true};

    RebootType m_scheduled_reboot{RebootType::None};

    Storecache read();

  public:
    SettingsStore();

    void setUsbMode(usb_mode_t mode);
    [[nodiscard]] usb_mode_t getUsbMode() const;

    void setLedBrightness(uint8_t brightness);
    [[nodiscard]] uint8_t getLedBrightness() const;

    void setLedAnimationSpeed(uint8_t speed);
    [[nodiscard]] uint8_t getLedAnimationSpeed() const;

    void setLedIdleMode(Peripherals::TouchSliderLeds::Config::IdleMode mode);
    [[nodiscard]] Peripherals::TouchSliderLeds::Config::IdleMode getLedIdleMode() const;

    void setLedTouchedMode(Peripherals::TouchSliderLeds::Config::TouchedMode mode);
    [[nodiscard]] Peripherals::TouchSliderLeds::Config::TouchedMode getLedTouchedMode() const;

    void setLedIdleColor(Peripherals::TouchSliderLeds::Config::Color color);
    [[nodiscard]] Peripherals::TouchSliderLeds::Config::Color getLedIdleColor() const;

    void setLedTouchedColor(Peripherals::TouchSliderLeds::Config::Color color);
    [[nodiscard]] Peripherals::TouchSliderLeds::Config::Color getLedTouchedColor() const;

    void setLedEnablePlayerColor(bool do_enable);
    [[nodiscard]] bool getLedEnablePlayerColor() const;

    void setLedEnablePdloaderSupport(bool do_enable);
    [[nodiscard]] bool getLedEnablePdloaderSupport() const;

    void setInputMirrorToDpad(bool do_mirror);
    [[nodiscard]] bool getInputMirrorToDpad() const;

    void scheduleReboot(bool bootsel = false);

    void store();
    void reset();
};
} // namespace Divacon::Utils

#endif // UTILS_SETTINGSSTORE_H_