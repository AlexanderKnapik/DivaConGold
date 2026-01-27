#include "hid.h"

#include "buttons/buttons.h"
#include "common/buttons_common.h"
#include "common/error.h"
#include "usb/keyboard.h"
#include "usb/usb_descriptors.h"

#include <tusb.h>

#include <stdint.h>

static enum error setup_task(void)
{
    if (tud_suspended()) {
        tud_remote_wakeup();
        return E_BUSY;
    }

    /* skip if hid is not ready yet */
    if (!tud_hid_ready()) {
        return E_BUSY;
    }

    return E_SUCCESS;
}

enum error hid_task(void (*fill_report_fn)(struct usb_report *, const struct buttons *),
                    const struct buttons *btns)
{
    enum error err = setup_task();

    if (err == E_SUCCESS) {
        struct usb_report report = {};
        fill_report_fn(&report, btns);

        if (!tud_hid_report(USB_REPORT_ID_KEYBOARD, report.data, report.len)) {
            err = E_HARDWARE;
        }
    }

    return err;
}

/* Invoked when sent REPORT successfully to host */
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint16_t len)
{
    (void)instance;
    (void)len;
    (void)report;
}

/*
 * Invoked when received GET_REPORT control request
 * Application must fill buffer report's content and return its length.
 * Return zero will cause the stack to STALL request
 */
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type,
                               uint8_t *buffer, uint16_t reqlen)
{
    // TODO not Implemented
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;

    return 0;
}

/*
 * Invoked when received SET_REPORT control request or
 * received data on OUT endpoint ( Report ID = 0, Type = 0 )
 */
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type,
                           uint8_t const *buffer, uint16_t bufsize)
{
    (void)instance;
    (void)buffer;

    if (report_type == HID_REPORT_TYPE_OUTPUT) {
        if (report_id == USB_REPORT_ID_KEYBOARD) {
            /* bufsize should be (at least) 1 */
            if (bufsize < 1) {
                return;
            }
        }
    }
}
