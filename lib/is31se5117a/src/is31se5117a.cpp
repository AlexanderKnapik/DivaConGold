#include "Is31se5117a.h"

#include "pico/time.h"

#include <array>

namespace {
constexpr uint8_t KEY_COUNT = 16;
} // namespace

Is31se5117a::Is31se5117a(uint8_t address, i2c_inst *i2c, uint8_t threshold, uint8_t hysteresis)
    : m_i2c(i2c), m_address(address) {

    // Reset
    writeRegister(Register::MAIN_CONTROL, 0x80);
    sleep_ms(1);

    // Make sure we are on page 0
    writeRegister(Register::SWITCH_PAGE, 0x00);

    // Set up filters
    // writeRegister(Register::RAW_COUNT_FILTER, 0x??)
    // writeRegister(Register::BASELINE_IIR_RATIO, 0x??)

    // Lock threshold ??

    // Raw count difference limit ??

    // Allow all keys to be triggered at one time
    writeRegister(Register::MULTI_TOUCH_KEY_CONFIGURE, 0x00);

    // Allow keys to be pressed indefinitely
    writeRegister(Register::MAX_DURATION_TIME, 0x00);

    // Disable shield
    writeRegister(Register::SHIELD_PIN_SELECT_1, 0x00);
    writeRegister(Register::SHIELD_PIN_SELECT_2, 0x00);

    // Disable buzzer
    writeRegister(Register::BUZZER_PIN_SELECT_1, 0x00);
    writeRegister(Register::BUZZER_PIN_SELECT_2, 0x00);

    // Disable GPIO
    writeRegister(Register::GPIO_PIN_SELECT_1, 0x00);
    writeRegister(Register::GPIO_PIN_SELECT_2, 0x00);

    // Disable Sliders
    writeRegister(Register::SLIDER1_KEY_SELECT_1, 0x00);
    writeRegister(Register::SLIDER1_KEY_SELECT_2, 0x00);
    writeRegister(Register::SLIDER2_KEY_SELECT_1, 0x00);
    writeRegister(Register::SLIDER2_KEY_SELECT_2, 0x00);

    // TKIII Configuration
    // writeRegister(Register::TKIII_CONTROL_1, 0x??);
    // writeRegister(Register::TKIII_CONTROL_2, 0x??);
    // writeRegister(Register::TKIII_CONTROL_3, 0x??);
    // writeRegister(Register::TKIII_CCHG, 0x??);
    // writeRegister(Register::TKIII_PUD, 0x??);

    // Clock setup
    // writeRegister(Register::SYSTEM_CLOCK_SELECT, 0x??);

    // Enable all keys
    writeRegister(Register::KEY_PIN_SELECT_1, 0xFF);
    writeRegister(Register::KEY_PIN_SELECT_2, 0xFF);

    // Page 1 registers

    setFingerThresholds(threshold);

    // Noise Threshold

    // Negative Noise Threshold

    // Low Baseline Reset

    setHystereses(hysteresis);

    // Debounce

    // Disable GPIO (again?)
    writeRegister(Register::GPIO_ENABLE_1, 0x00);
    writeRegister(Register::GPIO_ENABLE_2, 0x00);
}

uint16_t Is31se5117a::getTouched() {
    const uint16_t touched = readRegister16(Register::KEY_STATUS_1);

    return touched;
}
bool Is31se5117a::getTouched(uint8_t input) {
    if (input > KEY_COUNT - 1) {
        return false;
    }

    return (getTouched() & (1 << input)) != 0;
}

void Is31se5117a::setFingerThresholds(uint8_t threshold) {
    for (uint8_t i = 0; i < KEY_COUNT; ++i) {
        setFingerThreshold(i, threshold);
    }
}

void Is31se5117a::setFingerThreshold(uint8_t input, uint8_t threshold) {
    if (input > KEY_COUNT - 1) {
        return;
    }

    writeRegister(Register::KEY0_FINGER_THRESHOLD, threshold, input);
}

void Is31se5117a::setHystereses(uint8_t hysteresis) {
    for (uint8_t i = 0; i < KEY_COUNT; ++i) {
        setHysteresis(i, hysteresis);
    }
}

void Is31se5117a::setHysteresis(uint8_t input, uint8_t hysteresis) {
    if (input > KEY_COUNT - 1) {
        return;
    }

    writeRegister(Register::KEY0_HYSTERESIS, hysteresis, input);
}

void Is31se5117a::setDebounceCounts(uint8_t count) {
    for (uint8_t i = 0; i < KEY_COUNT; ++i) {
        setDebounceCount(i, count);
    }
}

void Is31se5117a::setDebounceCount(uint8_t input, uint8_t count) {
    if (input > KEY_COUNT - 1) {
        return;
    }

    writeRegister(Register::KEY0_ON_DEBOUNCE, count, input);
}

uint8_t Is31se5117a::getSignal(uint8_t input) {
    if (input > KEY_COUNT - 1) {
        return 0;
    }

    return readRegister8(Register::KEY0_SIGNAL, input);
}

uint16_t Is31se5117a::getRawCount(uint8_t input) {
    if (input > KEY_COUNT - 1) {
        return 0;
    }

    return readRegister16(Register::KEY0_RAW_COUNT_H, input * 2);
}

uint16_t Is31se5117a::getBaseline(uint8_t input) {
    if (input > KEY_COUNT - 1) {
        return 0;
    }

    return readRegister16(Register::KEY0_BASELINE_H, input * 2);
}

void Is31se5117a::setRegisterPage(uint16_t address) {
    if (address > 0x0009) {
        if (address > 0x00FF && m_current_page != RegisterPage::Page1) {
            doWriteRegister(static_cast<uint8_t>(Register::SWITCH_PAGE), 0x01);
            m_current_page = RegisterPage::Page1;
        } else if (address < 0x0100 && m_current_page != RegisterPage::Page0) {
            doWriteRegister(static_cast<uint8_t>(Register::SWITCH_PAGE), 0x00);
            m_current_page = RegisterPage::Page0;
        }
    }
}

uint8_t Is31se5117a::readRegister8(Is31se5117a::Register reg, uint8_t offset) {
    uint8_t result = 0;

    const uint16_t offset_addr = static_cast<uint16_t>(reg) + offset;
    const auto reg_addr = static_cast<uint8_t>(offset_addr & 0x00FF);

    setRegisterPage(offset_addr);

    i2c_write_blocking(m_i2c, m_address, &reg_addr, 1, true);
    i2c_read_blocking(m_i2c, m_address, &result, 1, false);

    return result;
}

uint16_t Is31se5117a::readRegister16(Is31se5117a::Register reg, uint8_t offset) {
    std::array<uint8_t, 2> result = {};

    const uint16_t offset_addr = static_cast<uint16_t>(reg) + offset;
    const auto reg_addr = static_cast<uint8_t>(offset_addr & 0x00FF);

    setRegisterPage(offset_addr);

    i2c_write_blocking(m_i2c, m_address, &reg_addr, 1, true);
    i2c_read_blocking(m_i2c, m_address, result.data(), 2, false);

    // High byte comes first for 16bit values
    return static_cast<uint16_t>(result[0]) << 8 | static_cast<uint16_t>(result[1]);
}

void Is31se5117a::writeRegister(Is31se5117a::Register reg, uint8_t value, uint8_t offset) {
    const uint16_t offset_addr = static_cast<uint16_t>(reg) + offset;

    setRegisterPage(offset_addr);
    doWriteRegister(static_cast<uint8_t>(offset_addr & 0x00FF), value);
}

void Is31se5117a::doWriteRegister(uint8_t addr, uint8_t value) {
    std::array data = {addr, value};
    i2c_write_blocking(m_i2c, m_address, data.data(), 2, false);
}