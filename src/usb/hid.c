#include "hid.h"

#include "common/buttons_common.h"
#include "common/error.h"
#include "common/slider_common.h"
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

enum error hid_task(void (*fill_report_fn)(struct usb_report *, const struct buttons *,
                                           const struct slider_state *),
                    const struct buttons *btns, const struct slider_state *slider)
{
    enum error err = setup_task();

    if (err == E_SUCCESS) {
        struct usb_report report = {};
        fill_report_fn(&report, btns, slider);

        if (!tud_hid_report(USB_REPORT_ID_KEYBOARD, report.data, report.len)) {
            err = E_HARDWARE;
        }
    }

    return err;
}

/* Callbacks are listed in "tinyusb/src/class/hid/hid_device.h" */

/*
 * Invoked when received GET_REPORT control request
 *
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
 * Invoked when received SET_REPORT control request or received data on
 * OUT endpoint ( Report ID = 0, Type = 0 ).
 */
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type,
                           uint8_t const *buffer, uint16_t bufsize)
{
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)bufsize;
}

void tud_hid_set_protocol_cb(uint8_t instance, uint8_t protocol)
{
    (void)instance;
    (void)protocol;
}

/*
 * Devices must enter low-power mode when bus is idle.
 *
 * Implement tud_suspend_cb() and tud_resume_cb() for power management.
 */
bool tud_hid_set_idle_cb(uint8_t instance, uint8_t idle_rate)
{
    (void)instance;
    (void)idle_rate;

    return true;
}

/* Invoked when sent REPORT successfully to host */
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint16_t len)
{
    (void)instance;
    (void)len;
    (void)report;
}

/* Invoked when a transfer wasn't successful */
void tud_hid_report_failed_cb(uint8_t instance, hid_report_type_t report_type,
                              uint8_t const *report, uint16_t xferred_bytes)
{
    (void)instance;
    (void)report_type;
    (void)report;
    (void)xferred_bytes;
}