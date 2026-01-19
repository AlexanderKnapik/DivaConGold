#include "pattern_buffer.h"
#include "pattern_buffer_pvt.h"

#include "common/error.h"
#include "common/util.h"
#include "slider/colour.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

pattern_buffer_handle_t pattern_buffer_open(const struct colour *colours, uint16_t size)
{
    if (!colours || size == 0) {
        return NULL;
    }

    pattern_buffer_handle_t pattern = malloc(sizeof(*pattern));

    if (pattern) {
        pattern->buf = colours;
        pattern->size = size;
        pattern->read = 0;
    }

    return pattern;
}

enum error pattern_buffer_close(pattern_buffer_handle_t pattern)
{
    if (!pattern) {
        return E_NULL_POINTER;
    }

    if (pattern) {
        free(pattern);
    }

    return E_SUCCESS;
}

static struct colour *pal_buf_copy(pattern_buffer_const_handle_t pattern, const uint16_t read_idx,
                                   struct colour *dest, uint16_t size)
{
    for (uint16_t i = 0; i < size; i++) {
        const struct colour *const src = &pattern->buf[read_idx + i];

        dest[i].r = src->r;
        dest[i].g = src->g;
        dest[i].b = src->b;
    }

    return dest;
}

uint16_t pattern_buffer_read(pattern_buffer_const_handle_t pattern, struct colour *dest,
                             uint16_t len)
{
    uint16_t bytes_read = 0;

    if (!dest || !pattern) {
        return bytes_read;
    }

    /*
     * Read the first chunk up starting from the buffer read index up to the
     * minimum of bytes requested, or until the end of the buffer
     */
    uint16_t chunk_size = MIN(len, (pattern->size - pattern->read));
    if (!pal_buf_copy(pattern, pattern->read, dest, chunk_size)) {
        return bytes_read;
    }

    bytes_read += chunk_size;

    /*
     * For the rest of the chunks, the read position of the pattern buffer can
     * be thought of as temporarily being in the 0 read index position.
     */
    while (bytes_read < len) {
        chunk_size = MIN(pattern->size, (len - bytes_read));

        if (!pal_buf_copy(pattern, 0, &dest[bytes_read], chunk_size)) {
            return bytes_read;
        }

        bytes_read += chunk_size;
    }

    return bytes_read;
}

uint16_t pattern_buffer_increment(pattern_buffer_handle_t pattern, uint16_t val)
{
    if (!pattern) {
        return 0;
    }

    /* Increment and wrap around the read index pointer for overflows */
    pattern->read += val;

    if (pattern->read >= pattern->size) {
        pattern->read %= pattern->size;
    }

    return pattern->read;
}
