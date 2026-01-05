#ifndef PERIPHERALS_TOUCHSLIDERLEDS_H_
#define PERIPHERALS_TOUCHSLIDERLEDS_H_

#include <algorithm>
#include <array>
#include <cstdint>
#include <optional>
#include <vector>

namespace Divacon::Peripherals {

class TouchSliderLeds {
  private:
    static constexpr size_t SEGMENT_COUNT = 32;

    static constexpr uint32_t PULSE_STEPS = 4096;
    static constexpr uint32_t RAINBOW_STEPS = 4096;
    static constexpr uint32_t FADE_STEPS = 2048;
    static constexpr uint32_t BLEND_STEPS = 128;

    static constexpr uint8_t PULSE_DIM_PCT_MIN = 40;
    static constexpr uint8_t PULSE_DIM_PCT_MAX = 100;

  public:
    struct Config {
        struct Color {
            uint8_t r;
            uint8_t g;
            uint8_t b;

            bool operator==(const Color &) const = default;
            bool operator!=(const Color &) const = default;
        };

        enum class IdleMode : uint8_t {
            Off,
            Static,
            Pulse,
            RainbowStatic,
            RainbowCycle,
        };

        enum class TouchedMode : uint8_t {
            Off,
            Idle,
            Touched,
            TouchedFade,
            TouchedIdle,
        };

        uint8_t led_pin;
        bool is_rgbw;
        bool reverse;
        uint16_t leds_per_segment;

        uint8_t brightness;
        uint8_t animation_speed;
        IdleMode idle_mode;
        TouchedMode touched_mode;
        Color idle_color;
        Color touched_color;

        bool enable_player_color;
        bool enable_pdloader_support;
    };

    using RawFrameMessage = std::array<Config::Color, SEGMENT_COUNT>;

  private:
    class AnimationStepper {
      private:
        uint32_t m_steps_until_advance;
        uint32_t m_current_steps{0};

      public:
        AnimationStepper(uint32_t steps_until_advance) : m_steps_until_advance(steps_until_advance) {};
        uint32_t getFrameCount(uint32_t steps);
    };

    Config m_config;
    uint32_t m_touched{0};

    struct {
        AnimationStepper stepper{PULSE_STEPS};
        uint8_t dim_percent{PULSE_DIM_PCT_MAX};
        int8_t advance_factor{-1};
    } m_pulse_state;

    struct {
        AnimationStepper stepper{RAINBOW_STEPS};
        size_t position{0};
    } m_rainbow_state;

    struct {
        AnimationStepper stepper{FADE_STEPS};
        std::array<uint8_t, SEGMENT_COUNT> percent{};
    } m_fade_state;

    struct {
        AnimationStepper stepper{BLEND_STEPS};
        uint8_t percent{100};
    } m_blend_state;

    std::vector<uint32_t> m_rendered_frame;
    uint32_t m_previous_frame_time{0};

    std::array<Config::Color, SEGMENT_COUNT> m_idle_buffer{};
    std::array<Config::Color, SEGMENT_COUNT> m_touched_buffer{};

    std::optional<Config::Color> m_player_color;

    bool m_raw_mode{false};

    void updateIdle(uint32_t steps);
    void updateTouched(uint32_t steps);

    void render(uint32_t steps);
    void show();

  public:
    TouchSliderLeds(const Config &config);

    void setBrightness(uint8_t brightness);
    void setAnimationSpeed(uint8_t speed);
    void setIdleMode(Config::IdleMode mode);
    void setTouchedMode(Config::TouchedMode mode);
    void setIdleColor(const Config::Color &color);
    void setTouchedColor(const Config::Color &color);
    void setEnablePlayerColor(bool do_enable);
    void setEnablePdloaderSupport(bool do_enable);

    void setTouched(uint32_t touched);
    void setPlayerColor(const Config::Color &color);

    void update();
    void update(const RawFrameMessage &frame);
};

} // namespace Divacon::Peripherals

#endif // PERIPHERALS_TOUCHSLIDERLEDS_H_