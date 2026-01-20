/*****************************************************************************/
/*                              buttons.c types                              */
/*****************************************************************************/
#ifndef BUTTONS_COMMON_H_
#define BUTTONS_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif // BUTTONS_COMMON_H_
