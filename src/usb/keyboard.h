#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common/buttons_common.h"
#include "usb/usb_handle.h"

#include <tusb.h>

#include <stdint.h>

enum error keyboard_init(struct usb_handle *usb);

#ifdef __cplusplus
}
#endif

#endif // KEYBOARD_H_
