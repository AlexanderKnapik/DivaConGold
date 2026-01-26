/*****************************************************************************/
/*                     Interface to the Touch Slider LEDs                    */
/*****************************************************************************/
#ifndef SLIDER_LEDS_H_
#define SLIDER_LEDS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common/colour_common.h"
#include "common/error.h"

#include <hardware/gpio.h>
#include <hardware/structs/pio.h>

#include <stdint.h>

typedef struct slider_leds_handle *slider_leds_handle_t;

enum slider_leds_pattern {
    SLIDER_LEDS_PATTERN_NONE = 0,
    SLIDER_LEDS_PATTERN_RAINBOW = 1,
};

enum slider_leds_animation {
    SLIDER_LEDS_ANIMATION_STATIC = 0,
    SLIDER_LEDS_ANIMATION_CYCLE = 1,
};

/**
 * @brief Configuration settings for the touch slider LEDs.
 *
 * @param pio The PIO instance to write to the SK6812B LEDs.
 * @param data_pin The GPIO pin to use to write to the SK6812B LEDs.
 * @param pattern The pre-set pattern to use for the animation.
 * @param animation The animation mode to use with the LEDs.
 * @param brightness The brightness setting of the LEDs. Maximum brightness is
 * rather bright and annoying.
 * @param touched_colour The colour the player touched electrode LEDs should
 * light up with.
 * @param player_colour I don't know what this does yet. I think it's meant
 * to be like a PS4 controller, where the slider is a solid colour?
 */
struct slider_leds_config {
    pio_hw_t *pio;
    uint8_t data_pin;
    enum gpio_slew_rate slew;
    enum gpio_drive_strength drive;

    enum slider_leds_pattern pattern;
    enum slider_leds_animation animation;
    uint8_t brightness;
    struct colour touched_colour;
    struct colour player_colour;
};

/**
 * @brief Initialise the slider LEDs and animation pattern to display.
 *
 * @todo Statically allocate the handle.
 *
 * @param [in] config Slider LEDs configuration settings.
 * @return Slider LEDs handle.
 */
slider_leds_handle_t slider_leds_open(const struct slider_leds_config *config);

/**
 * @brief De-initialise the slider LEDs and animation.
 *
 * Slider LEDs animation is dynamically allocated.
 *
 * @param [in,out] leds The Slider LEDs to de-initialise.
 */
void slider_leds_close(slider_leds_handle_t leds);

/**
 * @brief Display touched electrode LEDs and update the animation cycle.
 *
 * @param [in, out] leds
 * @return Error code
 * @retval E_SUCCESS Success.
 * @retval E_NULL_POINTER Null leds.
 * @retval E_IO Reading pattern buffer failed.
 */
enum error slider_leds_update(slider_leds_handle_t leds);

/**
 * @brief Update the touched electrodes to be displayed on update.
 *
 * The touched_state format is [31:0], where bit 0 is the right-most slider electrode
 * and bit 31 is the left-most slider electrode.
 *
 * @param [in, out] leds The slider leds to update the touched state of.
 * @param [in] touched_state The touched state the slider leds should display.
 * @return Error code.
 * @retval E_SUCCESS Success.
 * @retval E_NULL_POINTER Null leds.
 */
enum error slider_leds_ioctl_touched(slider_leds_handle_t leds, uint32_t touched_state);

#ifdef __cplusplus
}
#endif

#endif // SLIDER_LEDS_H_
