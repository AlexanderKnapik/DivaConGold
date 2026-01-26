/*****************************************************************************/
/*                                usb.c types                                */
/*****************************************************************************/
#ifndef USB_COMMON_H_
#define USB_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

enum usb_mode {
    USB_MODE_NONE = 0,
    USB_MODE_SWITCH_DIVACON,
    USB_MODE_SWITCH_HORIPAD,
    USB_MODE_DUALSHOCK3,
    USB_MODE_PS4_DIVACON,
    USB_MODE_PS4_COMPAT,
    USB_MODE_DUALSHOCK4,
    USB_MODE_XBOX360,
    USB_MODE_PDLOADER,
    USB_MODE_KEYBOARD,
    USB_MODE_MIDI,
    USB_MODE_DEBUG,
};

const char *usb_mode_to_string(enum usb_mode mode);

#ifdef __cplusplus
}
#endif

#endif // USB_COMMON_H_
