#include "usb.h"

#include "buttons/buttons.h"
#include "class/hid/hid.h"
#include "common/buttons_common.h"
#include "common/tusb_types.h"
#include "system/systick.h"
#include "tusb_config.h"
#include "usb_descriptors.h"

#include <bsp/board_api.h>
#include <tusb.h>

#include <stddef.h>
#include <stdint.h>

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum {
    BLINK_NOT_MOUNTED = 250,
    BLINK_MOUNTED = 1000,
    BLINK_SUSPENDED = 2500,
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

void led_blinking_task(void);
void hid_task(void);

/*------------- MAIN -------------*/
void usb_open(void)
{
    board_init();

    // init device stack on configured roothub port
    tud_init(BOARD_TUD_RHPORT);

    if (board_init_after_tusb) {
        board_init_after_tusb();
    }

    while (1) {
        tud_task(); // tinyusb device task
        led_blinking_task();

        hid_task();
    }
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
    blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
    blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void)remote_wakeup_en;
    blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
    blink_interval_ms = tud_mounted() ? BLINK_MOUNTED : BLINK_NOT_MOUNTED;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

static void send_hid_report(uint8_t report_id, uint32_t btn)
{
    // skip if hid is not ready yet
    if (!tud_hid_ready())
        return;

    switch (report_id) {
    case REPORT_ID_KEYBOARD: {
        // use to avoid send multiple consecutive zero report for keyboard
        static bool has_keyboard_key = false;

        if (btn) {
            uint8_t keycode[6] = {0};
            keycode[0] = HID_KEY_A;

            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
            has_keyboard_key = true;
        }
        else {
            // send empty key report if previously has key pressed
            if (has_keyboard_key)
                tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
            has_keyboard_key = false;
        }
    } break;

    case REPORT_ID_MOUSE: {
        int8_t const delta = 5;

        // no button, right + down, no scroll, no pan
        tud_hid_mouse_report(REPORT_ID_MOUSE, 0x00, delta, delta, 0, 0);
    } break;

    case REPORT_ID_CONSUMER_CONTROL: {
        // use to avoid send multiple consecutive zero report
        static bool has_consumer_key = false;

        if (btn) {
            // volume down
            uint16_t volume_down = HID_USAGE_CONSUMER_VOLUME_DECREMENT;
            tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &volume_down, 2);
            has_consumer_key = true;
        }
        else {
            // send empty key report (release key) if previously has key pressed
            uint16_t empty_key = 0;
            if (has_consumer_key)
                tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &empty_key, 2);
            has_consumer_key = false;
        }
    } break;

    case REPORT_ID_GAMEPAD: {
        // use to avoid send multiple consecutive zero report for keyboard
        static bool has_gamepad_key = false;

        hid_gamepad_report_t report = {
            .x = 0, .y = 0, .z = 0, .rz = 0, .rx = 0, .ry = 0, .hat = 0, .buttons = 0};

        if (btn) {
            report.hat = GAMEPAD_HAT_UP;
            report.buttons = GAMEPAD_BUTTON_A;
            tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));

            has_gamepad_key = true;
        }
        else {
            report.hat = GAMEPAD_HAT_CENTERED;
            report.buttons = 0;
            if (has_gamepad_key)
                tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));
            has_gamepad_key = false;
        }
    } break;

    default:
        break;
    }
}

// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void)
{
    // Poll every 10ms
    const uint32_t interval_ms = 10;
    static uint32_t start_ms = 0;

    if (board_millis() - start_ms < interval_ms)
        return; // not enough time
    start_ms += interval_ms;

    uint32_t const btn = board_button_read();

    // Remote wakeup
    if (tud_suspended() && btn) {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
    }
    else {
        // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
        send_hid_report(REPORT_ID_KEYBOARD, btn);
    }
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint16_t len)
{
    (void)instance;
    (void)len;

    uint8_t next_report_id = report[0] + 1u;

    if (next_report_id < REPORT_ID_COUNT) {
        send_hid_report(next_report_id, board_button_read());
    }
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
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

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type,
                           uint8_t const *buffer, uint16_t bufsize)
{
    (void)instance;

    if (report_type == HID_REPORT_TYPE_OUTPUT) {
        // Set keyboard LED e.g Capslock, Numlock etc...
        if (report_id == REPORT_ID_KEYBOARD) {
            // bufsize should be (at least) 1
            if (bufsize < 1)
                return;

            uint8_t const kbd_leds = buffer[0];

            if (kbd_leds & KEYBOARD_LED_CAPSLOCK) {
                // Capslock On: disable blink, turn led on
                blink_interval_ms = 0;
                board_led_write(true);
            }
            else {
                // Caplocks Off: back to normal blink
                board_led_write(false);
                blink_interval_ms = BLINK_MOUNTED;
            }
        }
    }
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void)
{
    static uint32_t start_ms = 0;
    static bool led_state = false;

    // blink is disabled
    if (!blink_interval_ms)
        return;

    // Blink every interval ms
    if (board_millis() - start_ms < blink_interval_ms)
        return; // not enough time
    start_ms += blink_interval_ms;

    board_led_write(led_state);
    led_state = 1 - led_state; // toggle
}

// void hid_task(void);

// void usb_task(void)
// {
//     tud_task();
//     hid_task();
// }

// void usb_open(void)
// {
//     board_init();

//     // init device stack on configured roothub port
//     tud_init(BOARD_TUD_RHPORT);

//     if (board_init_after_tusb) {
//         board_init_after_tusb();
//     }

//     while (1) {
//         usb_task();
//     }
// }

// //--------------------------------------------------------------------+
// // Device callbacks
// //--------------------------------------------------------------------+

// // Invoked when device is mounted
// void tud_mount_cb(void)
// {
// }

// // Invoked when device is unmounted
// void tud_umount_cb(void)
// {
// }

// // Invoked when usb bus is suspended
// // remote_wakeup_en : if host allow us  to perform remote wakeup
// // Within 7ms, device must draw an average of current less than 2.5 mA from bus
// void tud_suspend_cb(bool remote_wakeup_en)
// {
//     (void)remote_wakeup_en;
// }

// // Invoked when usb bus is resumed
// void tud_resume_cb(void)
// {
//     tud_mounted();
// }

// //--------------------------------------------------------------------+
// // USB HID
// //--------------------------------------------------------------------+

// void fill_key_code(const struct buttons *btns, uint8_t *keycode)
// {
//     if (btns->triangle) {
//         keycode[0] = HID_KEY_Z;
//         // keycode++;
//     }

//     // if (btns->square) {
//     //     *keycode = HID_KEY_X;
//     //     keycode++;
//     // }

//     // if (btns->cross) {
//     //     *keycode = HID_KEY_C;
//     //     keycode++;
//     // }

//     // if (btns->circle) {
//     //     *keycode = HID_KEY_V;
//     // }
// }

// static void send_hid_report(uint8_t report_id, const struct buttons *btns)
// {
//     // skip if hid is not ready yet
//     if (!tud_hid_ready())
//         return;

//     if (report_id == REPORT_ID_KEYBOARD) {
//         static bool has_keyboard_key = false;

//         if (btns) {
//             uint8_t keycode[6] = {};
//             // fill_key_code(btns, keycode);
//             keycode[0] = HID_KEY_Z;

//             tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
//             has_keyboard_key = true;
//         }
//         else {
//             if (has_keyboard_key) {
//                 tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
//                 has_keyboard_key = false;
//             }
//         }
//     }
// }

// // Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// // tud_hid_report_complete_cb() is used to send the next report after previous one is complete
// void hid_task(void)
// {
//     // Poll every 10ms
//     const uint32_t interval_ms = 10;
//     static uint32_t start_ms = 0;

//     if (board_millis() - start_ms < interval_ms)
//         return; // not enough time
//     start_ms += interval_ms;

//     // Remote wakeup
//     if (tud_suspended()) {
//         // Wake up host if we are in suspend mode
//         // and REMOTE_WAKEUP feature is enabled by host
//         tud_remote_wakeup();
//     }
//     else {
//         // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
//         send_hid_report(REPORT_ID_KEYBOARD, buttons_update());
//     }
// }

// // Invoked when sent REPORT successfully to host
// // Application can use this to send the next report
// // Note: For composite reports, report[0] is report ID
// void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint16_t len)
// {
//     (void)instance;
//     (void)len;

//     if (!report) {
//         return;
//     }

//     // const uint8_t report_id = report[0];
//     // send_hid_report(report_id, buttons_update());
// }

// // Invoked when received GET_REPORT control request
// // Application must fill buffer report's content and return its length.
// // Return zero will cause the stack to STALL request
// uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t
// report_type,
//                                uint8_t *buffer, uint16_t reqlen)
// {
//     // TODO not Implemented
//     (void)instance;
//     (void)report_id;
//     (void)report_type;
//     (void)buffer;
//     (void)reqlen;

//     return 0;
// }

// // Invoked when received SET_REPORT control request or
// // received data on OUT endpoint ( Report ID = 0, Type = 0 )
// void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type,
//                            uint8_t const *buffer, uint16_t bufsize)
// {
//     (void)instance;
//     (void)buffer;

//     if (report_type == HID_REPORT_TYPE_OUTPUT) {
//         // Set keyboard LED e.g Capslock, Numlock etc...
//         if (report_id == REPORT_ID_KEYBOARD) {
//             // bufsize should be (at least) 1
//             if (bufsize < 1) {
//                 return;
//             }
//         }
//     }
// }

// void hid_task(void)
// {
//     const uint32_t poll_interval_ms = 10;
//     const uint32_t current_tick_ms = systick_now_ms();
//     static uint32_t prev_tick_ms = 0;

//     if (systick_interval_ms(current_tick_ms, prev_tick_ms) < poll_interval_ms) {
//         return;
//     }

//     const struct buttons *btns = buttons_read();

//     if (tud_suspended() && btns) {
//         tud_remote_wakeup();
//     }
//     else {
//         send_hid_report(REPORT_ID_KEYBOARD, btns);
//     }

//     prev_tick_ms = current_tick_ms;
// }
