#include "palette_buffer.h"
#include "palette_buffer_pvt.h"

#include "common/error.h"
#include "common/util.h"
#include "slider/colour.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

palette_buffer_handle_t palette_buffer_open(const struct colour *colours, uint16_t size)
{
    if (!colours || size == 0) {
        return NULL;
    }

    palette_buffer_handle_t palette = malloc(sizeof(*palette));

    if (palette) {
        palette->buf = colours;
        palette->size = size;
        palette->read = 0;
    }

    return palette;
}

enum error palette_buffer_close(palette_buffer_handle_t palette)
{
    if (!palette) {
        return E_NULL_POINTER;
    }

    if (palette) {
        free(palette);
    }

    return E_SUCCESS;
}

static struct colour *pal_buf_copy(palette_buffer_const_handle_t palette, const uint16_t read_idx,
                                   struct colour *dest, uint16_t size)
{
    for (uint16_t i = 0; i < size; i++) {
        const struct colour *const src = &palette->buf[read_idx + i];

        dest[i].r = src->r;
        dest[i].g = src->g;
        dest[i].b = src->b;
    }

    return dest;
}

uint16_t palette_buffer_read(palette_buffer_const_handle_t palette, struct colour *dest,
                             uint16_t len)
{
    uint16_t bytes_read = 0;

    if (!dest || !palette) {
        return bytes_read;
    }

    /*
     * Read the first chunk up starting from the buffer read index up to the
     * minimum of bytes requested, or until the end of the buffer
     */
    uint16_t chunk_size = MIN(len, (palette->size - palette->read));
    if (!pal_buf_copy(palette, palette->read, dest, chunk_size)) {
        return bytes_read;
    }

    bytes_read += chunk_size;

    /*
     * For the rest of the chunks, the read position of the palette buffer can
     * be thought of as temporarily being in the 0 read index position.
     */
    while (bytes_read < len) {
        chunk_size = MIN(palette->size, (len - bytes_read));

        if (!pal_buf_copy(palette, 0, &dest[bytes_read], chunk_size)) {
            return bytes_read;
        }

        bytes_read += chunk_size;
    }

    return bytes_read;
}

uint16_t palette_buffer_increment(palette_buffer_handle_t palette, uint16_t val)
{
    if (!palette) {
        return 0;
    }

    /* Increment and wrap around the read index pointer for overflows */
    palette->read += val;

    if (palette->read >= palette->size) {
        palette->read %= palette->size;
    }

    return palette->read;
}
