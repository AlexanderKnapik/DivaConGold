/*****************************************************************************/
/*                   Controlling the four face button LEDs                   */
/*****************************************************************************/
#ifndef BUTTON_LED_H_
#define BUTTON_LED_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

enum button_led {
    BUTTON_LED_NONE = 0,
    BUTTON_LED_TRIANGLE,
    BUTTON_LED_SQUARE,
    BUTTON_LED_CROSS,
    BUTTON_LED_CIRCLE,
};

/**
 * @brief Initialise all button LED GPIOs.
 */
void button_led_init(void);

/**
 * @brief Read the current state of a single button LED.
 *
 * If state is NULL, the LED will still be read, just not stored.
 *
 * @param [in] led The LED to read.
 * @param [out] state Pointer to store the read state of the LED.
 * True = On, False = Off.
 * @return Error code.
 * @retval E_SUCCESS Success.
 * @retval E_INVALID_INPUT Invalid led.
 */
enum error button_led_read(enum button_led led, bool *state);

/**
 * @brief Set the state of a single button LED.
 *
 * @param [in] led The LED to write to.
 * @param [in] state The state to write.
 * @return Error code.
 * @retval E_SUCCESS Success.
 * @retval E_INVALID_INPUT Invalid led.
 */
enum error button_led_write(enum button_led led, bool state);

#ifdef __cplusplus
}
#endif

#endif // BUTTON_LED_H_
