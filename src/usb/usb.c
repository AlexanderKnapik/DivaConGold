#include "usb.h"

#include "common/error.h"
#include "system/systick.h"
#include "tusb_config.h"
#include "usb/hid.h"

#include <bsp/board_api.h>
#include <tusb.h>

#include <stdint.h>

#define USB_POLL_INTERVAL_ms (10U)

enum error usb_task(void)
{
    tud_task();

    const uint32_t current_tick_ms = systick_now_ms();
    static uint32_t prev_tick_ms = 0;

    if (systick_interval_ms(current_tick_ms, prev_tick_ms) < USB_POLL_INTERVAL_ms) {
        return E_BUSY;
    }

    const enum error err = hid_task();

    if (err == E_SUCCESS) {
        prev_tick_ms = current_tick_ms;
    }

    return err;
}

enum error usb_open(void)
{
    /* init device stack on configured roothub port */
    if (!tud_init(BOARD_TUD_RHPORT)) {
        return E_HARDWARE;
    }

    return E_SUCCESS;
}

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
