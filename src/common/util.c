#include "util.h"

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

uint8_t msb_u16(uint16_t val)
{
    /* Read from the msb first so that large values exit quicker. */
    for (uint8_t i = 0; i < UINT16_WIDTH; i++) {
        if ((bool)(val & (1 << (UINT16_WIDTH - 1)))) {
            return i;
        }
    }

    return 0;
}

uint8_t lsb_u16(uint16_t val)
{
    /* Read from the lsb first so that small values exit quicker */
    for (uint8_t i = 0; i < UINT16_WIDTH; i++) {
        if ((bool)(val & (1 << i))) {
            return i;
        }
    }

    return 0;
}

uint8_t max_n_bit_value_u8(uint8_t n)
{
    return (UINT8_MAX >> (UINT8_WIDTH - n));
}

uint16_t max_n_bit_value_u16(uint8_t n)
{
    return (UINT16_MAX >> (UINT16_WIDTH - n));
}

uint32_t max_n_bit_value_u32(uint8_t n)
{
    return (UINT32_MAX >> (UINT32_WIDTH - n));
}

uint64_t max_n_bit_value_u64(uint8_t n)
{
    return (UINT64_MAX >> (UINT64_WIDTH - n));
}
