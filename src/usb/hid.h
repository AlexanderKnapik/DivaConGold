#ifndef HID_H_
#define HID_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common/buttons_common.h"
#include "common/error.h"
#include "usb/usb_handle.h"

enum error hid_task(void (*fill_report_fn)(struct usb_report *, const struct buttons *,
                                           const struct slider_state *),
                    const struct buttons *btns, const struct slider_state *slider);

#ifdef __cplusplus
}
#endif

#endif // HID_H_
