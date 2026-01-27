#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common/buttons_common.h"

#include <tusb.h>

#include <stdint.h>

extern const tusb_desc_device_t keyboard_desc_device;
extern const uint8_t keyboard_desc_hid_report[];
extern const uint8_t keyboard_desc_configuration[];

enum error keyboard_send_report(const struct buttons *btns);

#ifdef __cplusplus
}
#endif

#endif // KEYBOARD_H_
