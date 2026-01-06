#include "systick.h"

#include <stdint.h>

uint32_t systick_now_ms(void)
{
    return 0;
}

uint32_t systick_interval_ms(uint32_t current_ms, uint32_t previous_ms)
{
    (void)current_ms;
    (void)previous_ms;

    return 0;
}

uint32_t systick_time_since_ms(uint32_t prev_tick_ms)
{
    (void)prev_tick_ms;

    return 0;
}
