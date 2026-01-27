#ifndef USB_H_
#define USB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common/buttons_common.h"
#include "common/error.h"
#include "common/usb_common.h"

typedef struct usb_handle *usb_handle_t;
typedef const struct usb_handle *usb_const_handle_t;

usb_handle_t usb_open(enum usb_mode mode);
enum error usb_write(usb_const_handle_t usb, const struct buttons *btns);
enum error usb_close(usb_handle_t usb);

#ifdef __cplusplus
}
#endif

#endif // USB_H_
