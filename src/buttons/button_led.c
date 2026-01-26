#include "button_led.h"

#include "common/bsp.h"
#include "common/error.h"

#include <hardware/gpio.h>

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Get the GPIO pin number given the specified LED enum value.
 *
 * @param [in] led The LED to get the GPIO pin of.
 * @param [out] pin Pointer to store the pin value.
 * @return Error code.
 */
static enum error get_led_pin(enum button_led led, uint8_t *pin)
{
    if (!pin) {
        return E_NULL_POINTER;
    }

    switch (led) {
    case BUTTON_LED_NONE:
        break;
    case BUTTON_LED_TRIANGLE:
        *pin = LED_TRIANGLE_Pin;
        break;
    case BUTTON_LED_SQUARE:
        *pin = LED_SQUARE_Pin;
        break;
    case BUTTON_LED_CIRCLE:
        *pin = LED_CIRCLE_Pin;
        break;
    case BUTTON_LED_CROSS:
        *pin = LED_CROSS_Pin;
        break;
    default:
        return E_INVALID_INPUT;
    }

    return E_SUCCESS;
}

static enum error init_led(enum button_led led)
{
    uint8_t pin = 0;
    const enum error err = get_led_pin(led, &pin);

    if (err == E_SUCCESS) {
        gpio_init(pin);
        gpio_pull_up(pin);
        gpio_set_dir(pin, true);
        gpio_put(pin, !((bool)BUTTON_LED_ACTIVE_LEVEL));
        gpio_set_slew_rate(pin, BUTTON_LED_SLEW_RATE);
        gpio_set_drive_strength(pin, BUTTON_LED_DRIVE_STRENGTH);
    }

    return err;
}

void button_led_init(void)
{
    init_led(BUTTON_LED_TRIANGLE);
    init_led(BUTTON_LED_SQUARE);
    init_led(BUTTON_LED_CIRCLE);
    init_led(BUTTON_LED_CROSS);
}

enum error button_led_read(enum button_led led, bool *state)
{
    uint8_t pin = 0;
    const enum error err = get_led_pin(led, &pin);

    if (state && err == E_SUCCESS) {
        *state = (gpio_get(pin) == (bool)BUTTON_LED_ACTIVE_LEVEL);
    }

    return err;
}

enum error button_led_write(enum button_led led, bool state)
{
    uint8_t pin = 0;
    const enum error err = get_led_pin(led, &pin);

    if (err == E_SUCCESS) {
        gpio_put(pin, (state == (bool)BUTTON_LED_ACTIVE_LEVEL));
    }

    return err;
}
