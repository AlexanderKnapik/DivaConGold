#ifndef PALETTE_BUFFER_PVT_H_
#define PALETTE_BUFFER_PVT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "slider/colour.h"

#include <stdint.h>

/**
 * @brief A read-only circular buffer for holding an animation colour palette.
 *
 * Not to be included by source code, this privately defined for unit test
 * access.
 *
 * @param buf A circular buffer of stored colour values.
 * @param size The maximum size of the circular buffer.
 * @param read The current position of the read pointer within the buffer.
 */
struct palette_buffer_handle {
    const struct colour *buf;
    uint16_t size;
    uint16_t read;
};

#ifdef __cplusplus
}
#endif

#endif // PALETTE_BUFFER_PVT_H_
