#include "buttons.h"

#include "buttons/button_led.h"
#include "common/bsp.h"
#include "common/buttons_common.h"
#include "common/error.h"
#include "common/util.h"

#include <hardware/gpio.h>

#include <stdint.h>

/* Consecutive readings required for a state change */
#define BUTTON_COUNTER_THRESHOLD (3U)

/**
 * @brief The internal state to initialise and debounce a button
 *
 * @param pin Button GPIO pin number.
 * @param led The LED associated with the button.
 * @param counter A counter value used for debouncing button activation
 */
struct button_state {
    const uint32_t pin;
    const enum button_led led;
    uint8_t counter;
};

struct button_states {
    struct button_state triangle;
    struct button_state square;
    struct button_state cross;
    struct button_state circle;

    struct button_state up;
    struct button_state down;
    struct button_state left;
    struct button_state right;

    struct button_state l1;
    struct button_state l2;
    struct button_state l3;

    struct button_state r1;
    struct button_state r2;
    struct button_state r3;

    struct button_state start;
    struct button_state select;
    struct button_state home;
};

static struct button_states button_states = {
    .triangle = {BUTTON_TRIANGLE_Pin, BUTTON_LED_TRIANGLE, 0},
    .square = {BUTTON_SQUARE_Pin, BUTTON_LED_SQUARE, 0},
    .cross = {BUTTON_CROSS_Pin, BUTTON_LED_CROSS, 0},
    .circle = {BUTTON_CIRCLE_Pin, BUTTON_LED_CIRCLE, 0},

    .up = {BUTTON_UP_Pin, BUTTON_LED_NONE, 0},
    .down = {BUTTON_DOWN_Pin, BUTTON_LED_NONE, 0},
    .left = {BUTTON_LEFT_Pin, BUTTON_LED_NONE, 0},
    .right = {BUTTON_RIGHT_Pin, BUTTON_LED_NONE, 0},

    .l1 = {BUTTON_L1_Pin, BUTTON_LED_NONE, 0},
    .l2 = {BUTTON_L2_Pin, BUTTON_LED_NONE, 0},
    .l3 = {BUTTON_L3_Pin, BUTTON_LED_NONE, 0},

    .r1 = {BUTTON_R1_Pin, BUTTON_LED_NONE, 0},
    .r2 = {BUTTON_R2_Pin, BUTTON_LED_NONE, 0},
    .r3 = {BUTTON_R3_Pin, BUTTON_LED_NONE, 0},

    .start = {BUTTON_START_Pin, BUTTON_LED_NONE, 0},
    .select = {BUTTON_SELECT_Pin, BUTTON_LED_NONE, 0},
    .home = {BUTTON_HOME_Pin, BUTTON_LED_NONE, 0},
};

/* Array to allow indexing through the button_states struct */
static struct button_state *button_states_array[] = {
    &button_states.triangle, &button_states.square, &button_states.cross, &button_states.circle,
    &button_states.up,       &button_states.down,   &button_states.left,  &button_states.right,
    &button_states.l1,       &button_states.l2,     &button_states.l3,    &button_states.r1,
    &button_states.r2,       &button_states.r3,     &button_states.start, &button_states.select,
    &button_states.home,
};

/*
 * The activated state of all the buttons. Publically accessible through the
 * buttons.h functions.
 */
static struct buttons active_state = {};

void buttons_init(void)
{
    button_led_init();

    const uint8_t num_buttons = ARRAY_SIZE(button_states_array);

    for (uint8_t idx = 0; idx < num_buttons; idx++) {
        struct button_state *button = button_states_array[idx];

        /*
         * Set the GPIO pin state. Enable the internal pull-up anyways despite
         * the hardware pull-ups for redundancy sake.
         */
        gpio_init(button->pin);
        gpio_pull_up(button->pin);
        gpio_set_dir(button->pin, false); /* input */
        gpio_set_slew_rate(button->pin, BUTTON_SLEW_RATE);
        gpio_set_drive_strength(button->pin, BUTTON_DRIVE_STRENGTH);

        /* Reset the counter value */
        button->counter = 0;
    }
}

static bool is_button_set(const struct button_state *button)
{
    return (button->counter >= BUTTON_COUNTER_THRESHOLD);
}

const struct buttons *buttons_read(void)
{
    active_state.triangle = is_button_set(&button_states.triangle);
    active_state.square = is_button_set(&button_states.square);
    active_state.cross = is_button_set(&button_states.cross);
    active_state.circle = is_button_set(&button_states.circle);

    active_state.l1 = is_button_set(&button_states.l1);
    active_state.l2 = is_button_set(&button_states.l2);
    active_state.l3 = is_button_set(&button_states.l3);

    active_state.r1 = is_button_set(&button_states.r1);
    active_state.r2 = is_button_set(&button_states.r2);
    active_state.r3 = is_button_set(&button_states.r3);

    active_state.start = is_button_set(&button_states.start);
    active_state.select = is_button_set(&button_states.select);
    active_state.home = is_button_set(&button_states.home);

    /*
     * Prevent opposing d-pad sides being pressed at the same time. If they are
     * both set, then just keep whatever the previous state was.
     */
    const bool up_state = is_button_set(&button_states.up);
    const bool down_state = is_button_set(&button_states.down);
    const bool left_state = is_button_set(&button_states.left);
    const bool right_state = is_button_set(&button_states.right);

    if (!(up_state && down_state)) {
        active_state.up = up_state;
        active_state.down = down_state;
    }

    if (!(left_state && right_state)) {
        active_state.left = left_state;
        active_state.right = right_state;
    }

    return &active_state;
}

static bool read_button_pin(const struct button_state *button)
{
    return (gpio_get(button->pin) == (bool)BUTTON_ACTIVE_LEVEL);
}

static void update_button_counter(struct button_state *button, bool state)
{
    if (state) {
        button->counter++;
    }
    else {
        button->counter = 0;
    }
}

const struct buttons *buttons_update(void)
{
    const uint8_t num_buttons = ARRAY_SIZE(button_states_array);

    for (uint8_t idx = 0; idx < num_buttons; idx++) {
        struct button_state *button = button_states_array[idx];
        update_button_counter(button, read_button_pin(button));
        button_led_write(button->led, is_button_set(button));
    }

    return buttons_read();
}
