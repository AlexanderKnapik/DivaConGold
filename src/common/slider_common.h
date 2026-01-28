#ifndef SLIDER_COMMON_H_
#define SLIDER_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

enum slider_direction {
    SLIDER_DIRECTION_NONE = 0,
    SLIDER_DIRECTION_LEFT,
    SLIDER_DIRECTION_RIGHT,
};

struct slider_state {
    uint32_t raw;
    enum slider_direction left;
    enum slider_direction right;
};

#ifdef __cplusplus
}
#endif

#endif // SLIDER_COMMON_H_
