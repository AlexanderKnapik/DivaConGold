#include "bpm_counter.h"

#include "common/buttons_common.h"
#include "common/systick.h"
#include "menu/bpm_buffer.h"

#include <stddef.h>
#include <stdint.h>

#define BUFFER_SIZE = (20U)
#define COMBO_PRESS_DELAY_MS (50U)

struct bpm_counter_handle {
    uint32_t timeout_ms;
    uint32_t previous_tick_ms;
    struct buttons prev_buttons;
};

struct bpm_counter_handle handle = {};

static void reset_handle(void)
{
    handle.previous_tick_ms = systick_now_ms();
    handle.prev_buttons.triangle = false;
    handle.prev_buttons.square = false;
    handle.prev_buttons.cross = false;
    handle.prev_buttons.circle = false;
}

void bpm_counter_init(uint32_t timeout_ms)
{
    handle.timeout_ms = timeout_ms;
    reset_handle();
}

static bool is_new_face_button_pressed(const struct buttons *buttons)
{
    if (!buttons) {
        return false;
    }

    return ((buttons->triangle && !handle.prev_buttons.triangle) || (buttons->square && !handle.prev_buttons.square) ||
            (buttons->cross && !handle.prev_buttons.cross) || (buttons->circle && !handle.prev_buttons.circle));
}

static void update_previous_buttons(const struct buttons *new)
{
    if (!new) {
        return;
    }

    handle.prev_buttons.triangle = new->triangle;
    handle.prev_buttons.square = new->square;
    handle.prev_buttons.cross = new->cross;
    handle.prev_buttons.circle = new->circle;
}

uint16_t bpm_counter_read(void)
{
    uint32_t bpm = 60000;
    bpm /= bpm_buffer_read_average();

    return bpm;
}

uint16_t bpm_counter_ioctl_update(const struct buttons *buttons)
{
    /*
     * How the firmware originally did it:
     *
     * It recorded the average time between the last 'WINDOW_SIZE' button presses.
     *
     * 1. Get the current systick and the previous systick the BPM was checked.
     * 2. If the difference is greater than the timeout period, reset the BPM.
     * 3. If the difference is smaller than the timeout, but greater than the
     *    double hit period and at least one face button has changed state,
     *    insert the difference into the buffer and calculate the bpm from the average
     *    of all buffer values.
     * 4. Update the previous systick value.
     * 5. Update the previous button states.
     *
     * So, essentially if you're hitting multiple buttons at the same time
     * (a combo), you just don't want each key to count as a separate press,
     * and to count it as one press instead.
     */
    const uint32_t current_tick_ms = systick_now_ms();
    const uint32_t interval_ms = systick_interval_ms(current_tick_ms, handle.previous_tick_ms);

    if (interval_ms > handle.timeout_ms) {
        bpm_buffer_clear();
        reset_handle();
    }

    if ((interval_ms > COMBO_PRESS_DELAY_MS) && is_new_face_button_pressed(buttons)) {
        bpm_buffer_write(interval_ms);
    }

    handle.previous_tick_ms = current_tick_ms;
    update_previous_buttons(buttons);

    return bpm_counter_read();
}
