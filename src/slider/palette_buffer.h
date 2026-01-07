#ifndef PALETTE_BUFFER_H_
#define PALETTE_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common/error.h"
#include "slider/colour.h"

#include <stdint.h>

typedef struct palette_buffer_handle *palette_buffer_handle_t;
typedef const struct palette_buffer_handle *palette_buffer_const_handle_t;

/**
 * @brief Initialise a palette buffer with an array of colours.
 *
 * The palette buffer is essentially a circular buffer that is filled up
 * completely after initialisation, with the read position requiring manual
 * incrementing. This is to allow a smaller colour palette to be copied to
 * a destination buffer while continuously looping over the window of colours.
 *
 * Initialise a palette buffer requires an uninitialised palette buffer
 * struct to be passed to palette_buffer_init()
 *
 * Dynamically allocated to the heap. Call pallete_buffer_close() to free the
 * memory.
 *
 * @param [in] colours The array of pre-defined colours that the palette buffer
 * should point to.
 * @param [in] size The size of the array of pre-defined colours.
 */
palette_buffer_handle_t palette_buffer_open(const struct colour *colours, uint16_t size);

/**
 * @brief De-initialise a palette buffer.
 *
 * @param [in] palette The palette buffer to de-initialise.
 * @return Error Code.
 */
enum error palette_buffer_close(palette_buffer_handle_t palette);

/**
 * @brief Read the colours from a palette buffer into a colours array.
 *
 * @param [in] palette The palette buffer to copy from.
 * @param [out] dest The array to copy from the palette into.
 * @param [in] len The amount of bytes to copy from the src palette into dest.
 * As the palette buffer is a circular buffer that continuously wraps, len nmay
 * be greater than the palette buffer size, but ensure that the destination
 * buffer is not overflown.
 * @return The amount of bytes read.
 */
uint16_t palette_buffer_read(palette_buffer_const_handle_t palette, struct colour *dest,
                             uint16_t len);

/**
 * @brief Manually increment the read pointer in the ciruclar palette buffer.
 *
 * Automatically handles the read pointer overflowing.
 *
 * @param [in] palette The palette buffer to increment.
 * @param [in] val The amount of bytes to increment by.
 * @return The new index of the read pointer.
 * @return 0 Null palette pointer.
 */
uint16_t palette_buffer_increment(palette_buffer_handle_t palette, uint16_t idx);

#ifdef __cplusplus
}
#endif

#endif // PALETTE_BUFFER_H_
