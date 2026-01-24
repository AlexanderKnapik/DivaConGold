/*****************************************************************************/
/*                            LED Strip Animation                            */
/*****************************************************************************/
#ifndef ANIMATION_H_
#define ANIMATION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common/error.h"
#include "slider/colour.h"

#include <stdint.h>

enum animation_mode {
    ANIMATION_MODE_STATIC = 0,
    ANIMATION_MODE_CYCLE,
};

typedef struct animation_handle *animation_handle_t;
typedef const struct animation_handle *animation_const_handle_t;

/**
 * @brief Initiliase an LED animation given an array of colours.
 *
 * @todo Statically allocate the animations.
 *
 * @param [in] mode Whether the animation should scroll through the given
 * colours, or be a static pattern.
 * @param [in] speed How many colours the animation should be incremented by
 * for each read. Generally kept at a value of 1.
 * @param [in] colours The array of colours to define the animation.
 * @param [in] size The size of colours.
 * @return animation_handle_t Animation handle.
 * @retval NULL The animation could not be intitialised.
 */
animation_handle_t animation_open(enum animation_mode mode, uint8_t speed,
                                  const struct colour *colours, uint16_t size);

/**
 * @brief De-initialise the LED animation.
 *
 * @param [in,out] animation The animation handle to de-initialise.
 */
void animation_close(animation_handle_t animation);

/**
 * @brief Set the speed of the animation.
 *
 * The speed is how many steps within the colours index that the animation
 * should increment by, each time animation_read() is called.
 *
 * @param [in,out] animation The animation to set the speed of.
 * @param [in] speed The index increment amount.
 * @return Error code.
 * @retval E_SUCCESS Success.
 * @retval E_NULL_POINTER Null animation.
 */
enum error animation_ioctl_speed(animation_handle_t animation, uint8_t speed);

/**
 * @brief Set the mode of the animation.
 *
 * @param [in, out] animation The animation to set the mode of.
 * @param [in] mode The mode to set the animation to.  ANIMATION_MODE_CYCLE
 * will cycle through colours the animation was initialised with, like a
 * circular buffer. ANIMATION_MODE_STATIC will not scroll through each time it is read.
 * @return Error code.
 * @retval E_SUCCESS Success.
 * @retval E_NULL_POINTER Null animation.
 * @retval E_INVALID_INPUT Invalid mode.
 */
enum error animation_ioctl_mode(animation_handle_t animation, enum animation_mode mode);

/**
 * @brief Read from the animation and copy the contents to a buffer.
 *
 * @param [in, out] animation The animation to read.
 * @param [out] dest Pointer to copy the animation frame to.
 * @param [in] size The amount of bytes to copy into dest. This value can be
 * greater than the size of the array that the animation was initialised with,
 * the animation will simply wrap around back to the beginning when copying to
 * the destination buffer.
 * @return Error code.
 * @retval E_SUCCESS Success.
 * @retval E_NULL_POINTER Null animation.
 */
enum error animation_read(animation_handle_t animation, struct colour *dest, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif // ANIMATION_H_
