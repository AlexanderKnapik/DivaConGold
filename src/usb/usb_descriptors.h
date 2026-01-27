#ifndef USB_DESCRIPTORS_H_
#define USB_DESCRIPTORS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* TODO: Buy real vendor and product IDs */
#define USB_VID 0xCAFE         /* Vendor ID */
#define USB_PID 0x3901         /* Product ID */
#define USB_DEV_VER_BCD 0x0100 /* Release number in BCD */
#define USB_SPEC_VER_BCD 0x0200

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)
#define EPNUM_HID 0x81

#define USB_POWER_MAX_mA (500U)

/* String indexes for struct tusb_desc_device_t */
enum usb_str_id {
    USB_STR_ID_LANGID = 0,
    USB_STR_ID_MANUFACTURER,
    USB_STR_ID_PRODUCT,
    USB_STR_ID_SERIAL,
};

enum usb_report_id {
    USB_REPORT_ID_KEYBOARD = 1,
    USB_REPORT_ID_GAMEPAD,
};

/* TODO: Figure out ITF_NUM means */
enum usb_itf {
    USB_ITF_NUM_HID = 0,
    USB_ITF_NUM_TOTAL = 1,
};

#ifdef __cplusplus
}
#endif

#endif /* USB_DESCRIPTORS_H_ */
