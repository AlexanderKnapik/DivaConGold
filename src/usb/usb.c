#include "usb.h"

#include "buttons/buttons.h"
#include "common/error.h"
#include "common/usb_common.h"
#include "device/usbd.h"
#include "system/systick.h"
#include "tusb_config.h"
#include "usb/keyboard.h"
#include "usb/usb_handle.h"

#include <bsp/board_api.h>
#include <tusb.h>

#include <stddef.h>
#include <stdint.h>

#define USB_POLL_INTERVAL_ms (1U)

static struct usb_handle active_usb_handle = {};

/*****************************************************************************/
/*                            USB Driver Functions                           */
/*****************************************************************************/
enum error usb_write(usb_const_handle_t usb, const struct buttons *btns,
                     const struct slider_state *slider)
{
    /*
     * Must be called regularly (typically less than 1ms intervals) to ensure
     * all USB events are processed in task context, where application callbacks
     * also execute.
     */
    tud_task();

    if (!btns || !slider) {
        return E_NULL_POINTER;
    }

    const uint32_t current_tick_ms = systick_now_ms();
    static uint32_t prev_tick_ms = 0;

    if (systick_interval_ms(current_tick_ms, prev_tick_ms) < USB_POLL_INTERVAL_ms) {
        return E_BUSY;
    }

    const enum error err = usb->send_report_cb(btns, slider);

    if (err == E_SUCCESS) {
        prev_tick_ms = current_tick_ms;
    }

    return err;
}

usb_handle_t usb_open(enum usb_mode mode)
{
    struct usb_handle *const usb = &active_usb_handle;

    /* If the active USB interface is already opened, then close it */
    if (usb_close(usb) != E_SUCCESS) {
        return NULL;
    }

    /* init device stack on configured roothub port */
    if (!tud_init(BOARD_TUD_RHPORT)) {
        return NULL;
    }

    enum error err = E_SUCCESS;

    if (mode == USB_MODE_KEYBOARD) {
        err = keyboard_init(usb);
    }

    if (err != E_SUCCESS) {
        return NULL;
    }

    return usb;
}

enum error usb_close(usb_handle_t usb)
{
    if (!tud_deinit(BOARD_TUD_RHPORT)) {
        return E_GENERIC;
    }

    if (usb) {
        *usb = (struct usb_handle){};
    }

    return E_SUCCESS;
}

/*****************************************************************************/
/*                             Tiny USB Callbacks                            */
/*****************************************************************************/
/* Invoked when device is mounted */
void tud_mount_cb(void)
{
}

/* Invoked when device is unmounted */
void tud_umount_cb(void)
{
}

/*
 * Invoked when usb bus is suspended
 * remote_wakeup_en : if host allow us  to perform remote wakeup
 * Within 7ms, device must draw an average of current less than 2.5 mA from bus
 */
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void)remote_wakeup_en;
}

/* Invoked when usb bus is resumed */
void tud_resume_cb(void)
{
}

/*
 * Invoked when received GET DEVICE DESCRIPTOR
 * Application return pointer to descriptor
 */
uint8_t const *tud_descriptor_device_cb(void)
{
    return (uint8_t const *)active_usb_handle.desc_device;
}

/*
 * Invoked when received GET HID REPORT DESCRIPTOR
 * Application return pointer to descriptor
 * Descriptor contents must exist long enough for transfer to complete
 */
uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance)
{
    (void)instance;

    return active_usb_handle.desc_hid_report;
}

/*
 * Invoked when received GET CONFIGURATION DESCRIPTOR
 * Application return pointer to descriptor
 * Descriptor contents must exist long enough for transfer to complete
 */
uint8_t const *tud_descriptor_configuration_cb(uint8_t index)
{
    (void)index; /* for multiple configurations */

    return active_usb_handle.desc_configuration;
}
