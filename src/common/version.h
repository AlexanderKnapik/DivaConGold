#ifndef VERSION_H_
#define VERSION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

struct version {
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
};

extern const struct version version;

#ifdef __cplusplus
}
#endif

#endif // VERSION_H_
