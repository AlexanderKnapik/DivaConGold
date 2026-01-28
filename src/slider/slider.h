/*****************************************************************************/
/*                   Control Multiple MPR121s as One Device                  */
/*****************************************************************************/
#ifndef SLIDER_H_
#define SLIDER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common/error.h"
#include "common/slider_common.h"
#include "peripheral/i2c.h"

#include <stdint.h>

typedef struct slider_handle *slider_handle_t;
typedef const struct slider_handle *slider_const_handle_t;

/**
 * @brief Initialise the slider I2C interfaces and MPR121 touch sensors.
 *
 * The returned slider is a statically allocated singleton. Attempting to open
 * multiple sliders will overwrite the previously opened slider.
 *
 * @return Slider instance.
 * @retval NULL Slider failed to initialise.
 */
slider_handle_t slider_open(const struct i2c_config *i2c);

/**
 * @brief De-initialise the slider I2C interfaces and MPR121 touch sensors.
 */
void slider_close(slider_handle_t slider);

/**
 * @brief Read the state of the slider electrodes.
 *
 * There are 32 electrodes in total that make up the slider. How each bit is
 * mapped is that essentially each physical slider position can be thought of
 * as a bit, so the left side of the slider becomes the upper bits of the bit
 * mask, and the right side becomes the lower bits. E.g.
 *
 * 3322 2222 2222 1111 1111 11
 * 1098 7654 3210 9876 5432 1098 7654 3210
 * ---------------------------------------
 * 1111 0000 0000 0000 0000 0000 0000 0000
 * ---------------------------------------
 *
 * In this circumstance, the user is holding down the four left-most electrodes
 * of the slider at once, this results in a read state of 0xF000 0000.
 *
 * @todo Update the docstring for slider_state.
 *
 * @param [in] slider The slider to read from.
 * @param [out] state Pointer to store the active slider electrode bitmask.
 * If NULL, the MPR121s are still read, but the result isn't stored.
 * @return Error code.
 * @retval E_SUCCESS Success.
 * @retval E_NULL_POINTER Attempted to read from an un-initialised MPR121, or
 * write to an un-initialised slider LEDs handle.
 * @retval E_HARDWARE An MPR121 has an over-current fault.
 * @retval E_TIMEOUT Reading from an MPR121 timed out.
 * @retval E_IO Reading from an MPR121 only partially completed.
 */
enum error slider_read(slider_handle_t slider, struct slider_state *state);

#ifdef __cplusplus
}
#endif

#endif // SLIDER_H_
