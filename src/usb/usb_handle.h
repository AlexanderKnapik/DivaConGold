#ifndef USB_HANDLE_H_
#define USB_HANDLE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common/buttons_common.h"
#include "common/error.h"
#include "common/slider_common.h"

#include <common/tusb_types.h>

#include <stdint.h>

struct usb_report {
    uint8_t len;
    uint8_t data[32];
};

struct usb_handle {
    enum error (*send_report_cb)(const struct buttons *, const struct slider_state *);
    const tusb_desc_device_t *desc_device;
    const uint8_t *desc_hid_report;
    const uint8_t *desc_configuration;
};

#ifdef __cplusplus
}
#endif

#endif // USB_HANDLE_H_
