/*****************************************************************************/
/*          Circular Buffer for storing current BPM interval values          */
/*****************************************************************************/

#include "bpm_buffer.h"

#include "common/error.h"

#include <stddef.h>
#include <stdint.h>

#define BUFFER_SIZE (20U)

struct buffer {
    size_t idx; /* Index of the head of the circular buffer */
    size_t len;
    uint32_t data[BUFFER_SIZE];
};

static struct buffer buffer = {};

static void increment_buffer_length(struct buffer *buf)
{
    if (buf && buf->len < BUFFER_SIZE) {
        buf->len++;
    }
}

static void increment_buffer_index(struct buffer *buf)
{
    if (buf && buf->idx < BUFFER_SIZE) {
        buf->idx++;
    }
    else {
        buf->idx = 0;
    }
}

void bpm_buffer_write(uint32_t val)
{
    buffer.data[buffer.idx] = val;
    increment_buffer_length(&buffer);
    increment_buffer_index(&buffer);
}

void bpm_buffer_clear(void)
{
    for (uint8_t idx = 0; idx < BUFFER_SIZE; idx++) {
        buffer.data[idx] = 0;
    }

    buffer.len = 0;
}

uint32_t bpm_buffer_read_average(void)
{
    uint32_t avg = 0;

    for (size_t idx = 0; idx < buffer.len; idx++) {
        avg += buffer.data[idx];
    }

    avg /= buffer.len;

    return avg;
}
