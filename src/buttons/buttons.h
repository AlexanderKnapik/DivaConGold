#ifndef BUTTONS_H_
#define BUTTONS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common/error.h"

#include <stdbool.h>

struct buttons {
    bool triangle;
    bool square;
    bool cross;
    bool circle;

    bool up;
    bool down;
    bool left;
    bool right;

    bool l1;
    bool l2;
    bool l3;

    bool r1;
    bool r2;
    bool r3;

    bool start;
    bool select;
    bool home;
};

/**
 * @brief Initialise the GPIO pins of the buttons.
 */
void buttons_init();

/**
 * @brief Read the current state of the pressed buttons.
 */
const struct buttons *buttons_read(void);

/**
 * @brief Update, then read back the new state of the pressed buttons.
 *
 * @return The current state of the buttons.
 */
const struct buttons *buttons_ioctl_update(void);

#ifdef __cplusplus
}
#endif

#endif // BUTTONS_H_
