/*****************************************************************************/
/*         Circular Buffer for Storing an LED Strip Animation Pattern        */
/*****************************************************************************/
#ifndef PATTERN_BUFFER_H_
#define PATTERN_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common/error.h"
#include "slider/colour.h"

#include <stdint.h>

typedef struct pattern_buffer_handle *pattern_buffer_handle_t;
typedef const struct pattern_buffer_handle *pattern_buffer_const_handle_t;

/**
 * @brief Initialise a pattern buffer with an array of colours.
 *
 * The pattern buffer is essentially a circular buffer that is filled up
 * completely after initialisation, with the read position requiring manual
 * incrementing. This is to allow a smaller colour pattern to be copied to
 * a destination buffer while continuously looping over the window of colours.
 *
 * Initialise a pattern buffer requires an uninitialised pattern buffer
 * struct to be passed to pattern_buffer_init()
 *
 * Dynamically allocated to the heap. Call pallete_buffer_close() to free the
 * memory.
 *
 * @param [in] colours The array of pre-defined colours that the pattern buffer
 * should point to.
 * @param [in] size The size of the array of pre-defined colours.
 */
pattern_buffer_handle_t pattern_buffer_open(const struct colour *colours, uint16_t size);

/**
 * @brief De-initialise a pattern buffer.
 *
 * @param [in] pattern The pattern buffer to de-initialise.
 */
void pattern_buffer_close(pattern_buffer_handle_t pattern);

/**
 * @brief Read the colours from a pattern buffer into a colours array.
 *
 * @param [in] pattern The pattern buffer to copy from.
 * @param [out] dest The array to copy from the pattern into.
 * @param [in] len The amount of bytes to copy from the src pattern into dest.
 * As the pattern buffer is a circular buffer that continuously wraps, len nmay
 * be greater than the pattern buffer size, but ensure that the destination
 * buffer is not overflown.
 * @return The amount of bytes read.
 */
uint16_t pattern_buffer_read(pattern_buffer_const_handle_t pattern, struct colour *dest,
                             uint16_t len);

/**
 * @brief Manually increment the read pointer in the ciruclar pattern buffer.
 *
 * Automatically handles the read pointer overflowing.
 *
 * @param [in] pattern The pattern buffer to increment.
 * @param [in] val The amount of bytes to increment by.
 * @return The new index of the read pointer.
 * @return 0 Null pattern pointer.
 */
uint16_t pattern_buffer_increment(pattern_buffer_handle_t pattern, uint16_t idx);

#ifdef __cplusplus
}
#endif

#endif // PATTERN_BUFFER_H_
