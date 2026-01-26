#ifndef DISPLAY_H_
#define DISPLAY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "peripheral/i2c.h"

#include <stdint.h>

/**
 * @brief The different display menu modes.
 *
 * @param DISPLAY_MODE_IDLE The initial boot mode displaying user information.
 * @param DISPLAY_MODE_MENU The settings menu to configure the controller.
 */
enum display_mode {
    DISPLAY_MODE_IDLE = 0,
    DISPLAY_MODE_MENU,
};

/**
 * @brief The display initialisation and configuration settings
 *
 * @param height The display height in pixels.
 * @param width The display width in pixels.
 * @param i2c_block Memory mapped i2c instance of the microcontroller hardware.
 * @param i2c_address The I2C address of the display.
 * @param i2c_speed_hz The speed of the I2C bus in Hz.
 * @param sda_pin The I2C SDA GPIO pin number
 * @param scl_pin The I2C SCL GPIO pin number
 */
struct display_config {
    uint16_t width;
    uint16_t height;
    struct i2c_config i2c;
};

/* Opaque pointers for the display handles */
typedef struct display_handle *display_handle_t;
typedef const struct display_handle *display_const_handle_t;

/**
 * @brief Initialise the OLED display & I2C interface of the controller.
 *
 * @param [in] config The configuration values to initialise with.
 * @return Handle for configuring the display state.
 * @retval NULL The display could not be initialised.
 */
display_handle_t display_open(const struct display_config *config);

/**
 * @brief De-initialise the OLED display & I2C interface of the controller.
 *
 * @param [in] display The display handle to de-initialise.
 * @return Error code.
 */
enum error display_close(display_handle_t display);

/**
 * @brief Set the slider touched state that the display should show.
 *
 * Shown as boxes at the bottom of the screen while in the DISPLAY_MODE_IDLE
 * mode.
 *
 * @param [in] display The display handle.
 * @param [in] touched_state The touched state to display.
 * @return enum error
 */
enum error display_ioctl_touched(display_handle_t display, uint32_t touched_state);

/**
 * @brief Change the settings mode of the display.
 *
 * Requires display_ioctl_update() to be called in order to display the new
 * mode.
 *
 * @param [in] display The display handle.
 * @param [in] mode The display mode to change to.
 * @return Error code.
 */
enum error display_ioctl_mode(display_handle_t display, enum display_mode mode);

/**
 * @brief Update the current displayed information.
 *
 * This is required for example to display new information to the user such as
 * an updated touch slider or BPM state, but also if changing the mode of the
 * display. Poll this function in accordance with the required refresh rate.
 *
 * @param [in] display The display handle.
 * @return Error code.
 */
enum error display_update(display_handle_t display);

#ifdef __cplusplus
}
#endif

#endif // DISPLAY_H_
