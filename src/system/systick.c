#include "systick.h"

#include <pico/time.h>

#include <stdint.h>

#define SYSTICK_MAX UINT32_MAX

/*
 * 64 bit timer incrementing every microsecond which then gets truncated to a
 * 32 bit millisecond value. Overflows in 8.17 years.
 */
uint32_t systick_now_ms(void)
{
    return to_ms_since_boot(get_absolute_time());
}

uint32_t systick_interval_ms(uint32_t now_ms, uint32_t since_ms)
{
    if (now_ms >= since_ms) {
        return (now_ms - since_ms);
    }

    return (now_ms + (1 + SYSTICK_MAX - since_ms));
}

uint32_t systick_time_since_ms(uint32_t since_ms)
{
    return (systick_interval_ms(systick_now_ms(), since_ms));
}
