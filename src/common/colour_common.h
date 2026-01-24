/*****************************************************************************/
/*                               colour.c types                              */
/*****************************************************************************/
#ifndef COLOUR_COMMON_H_
#define COLOUR_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

struct colour {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

#ifdef __cplusplus
}
#endif

#endif // COLOUR_COMMON_H_
