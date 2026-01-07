#ifndef COLOUR_H_
#define COLOUR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

struct colour {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

extern const struct colour rainbow_colours[];

#ifdef __cplusplus
}
#endif

#endif // COLOUR_H_
