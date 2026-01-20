/*****************************************************************************/
/*                    Reading the controller face buttons                    */
/*****************************************************************************/
#ifndef BUTTONS_H_
#define BUTTONS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common/buttons_common.h"

#include <stdbool.h>

/**
 * @brief Initialise the GPIO pins of the buttons.
 */
void buttons_init();

/**
 * @brief Read the current state of the pressed buttons.
 *
 * @return The current state of the buttons.
 */
const struct buttons *buttons_read(void);

/**
 * @brief Update, then read back the new state of the pressed buttons.
 *
 * @return The current state of the buttons.
 */
const struct buttons *buttons_update(void);

#ifdef __cplusplus
}
#endif

#endif // BUTTONS_H_
