#include "keyboard.h"

#include "common/buttons_common.h"
#include "common/error.h"
#include "common/tusb_types.h"
#include "common/util.h"
#include "hid.h"
#include "usb/usb_descriptors.h"
#include "usb/usb_handle.h"

#include <tusb.h>

#include <stddef.h>
#include <stdint.h>

#define KEYBOARD_DESC_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)

static const tusb_desc_device_t keyboard_desc_device = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = USB_SPEC_VER_BCD,
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor = USB_VID,
    .idProduct = USB_PID,
    .bcdDevice = USB_DEV_VER_BCD,

    .iManufacturer = USB_STR_ID_MANUFACTURER,
    .iProduct = USB_STR_ID_PRODUCT,
    .iSerialNumber = USB_STR_ID_SERIAL,

    .bNumConfigurations = 0x01,
};

/* TODO: Remove magic numbers */
static const uint8_t keyboard_desc_hid_report[] = {
    0x05, 0x01,       // Usage Page (Generic Desktop Ctrls)
    0x09, 0x06,       // Usage (Keyboard)
    0xA1, 0x01,       // Collection (Application)
    0x85, 0x01,       //   Report ID (1)
    0x05, 0x07,       //   Usage Page (Kbrd/Keypad)
    0x19, 0x00,       //   Usage Minimum (0x00)
    0x2A, 0xFF, 0x00, //   Usage Maximum (0xFF)
    0x15, 0x00,       //   Logical Minimum (0)
    0x25, 0x01,       //   Logical Maximum (1)
    0x75, 0x01,       //   Report Size (1)
    0x96, 0x00, 0x01, //   Report Count (256)
    0x81, 0x02,       //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,             // End Collection
};

static const uint8_t keyboard_desc_configuration[] = {
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, USB_ITF_NUM_TOTAL, USB_STR_ID_LANGID, KEYBOARD_DESC_LEN,
                          TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, USB_POWER_MAX_mA),

    // Interface number, string index, protocol, report descriptor len, EP In address, size &
    // polling interval
    TUD_HID_DESCRIPTOR(USB_ITF_NUM_HID, 0, HID_ITF_PROTOCOL_KEYBOARD,
                       sizeof(keyboard_desc_hid_report), EPNUM_HID, CFG_TUD_HID_EP_BUFSIZE, 5),
};

/**
 * @brief Fill a bit position within the report for a given button and key.
 *
 * Essentially, the report is a look-up table of all possible HID key codes
 * from 0 to 255. What this does is set a bit for a given key index depending
 * on the button state.
 *
 * @param [out] report Pointer to the USB report look-up table.
 * @param [in] btn The state of the button press.
 * @param [in] key The look-up table index.
 */
static void set_key(struct usb_report *report, bool btn, uint8_t key)
{
    if (btn) {
        /* report->keycode(key / 8) |= 1 << (key % 8) */
        report->data[key >> 3] |= 1 << (key & (UINT8_WIDTH - 1));
    }
}

/* TODO: Slider State */
static void keyboard_fill_report(struct usb_report *report, const struct buttons *btns)
{
    if (report && btns) {
        report->len = 32;

        set_key(report, btns->triangle, HID_KEY_Z);
        set_key(report, btns->square, HID_KEY_X);
        set_key(report, btns->cross, HID_KEY_C);
        set_key(report, btns->circle, HID_KEY_V);

        set_key(report, btns->up, HID_KEY_ARROW_UP);
        set_key(report, btns->down, HID_KEY_ARROW_DOWN);
        set_key(report, btns->left, HID_KEY_ARROW_LEFT);
        set_key(report, btns->right, HID_KEY_ARROW_RIGHT);

        set_key(report, btns->start, HID_KEY_ENTER);
        set_key(report, btns->select, HID_KEY_F1);
        set_key(report, btns->home, HID_KEY_ESCAPE);

        set_key(report, btns->l1, HID_KEY_A);
        set_key(report, btns->l2, HID_KEY_S);
        set_key(report, btns->l3, HID_KEY_D);

        set_key(report, btns->r1, HID_KEY_L);
        set_key(report, btns->r2, HID_KEY_K);
        set_key(report, btns->r3, HID_KEY_J);
    }
}

static enum error keyboard_send_report_cb(const struct buttons *btns)
{
    return hid_task(keyboard_fill_report, btns);
}

enum error keyboard_init(struct usb_handle *usb)
{
    if (!usb) {
        return E_NULL_POINTER;
    }

    *usb = (struct usb_handle){
        .send_report_cb = keyboard_send_report_cb,
        .desc_device = &keyboard_desc_device,
        .desc_hid_report = keyboard_desc_hid_report,
        .desc_configuration = keyboard_desc_configuration,
    };

    return E_SUCCESS;
}