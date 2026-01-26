#include "usb_common.h"

#include <stddef.h>
#include <stdint.h>

const char *usb_mode_to_string(enum usb_mode mode)
{
    switch (mode) {
    case USB_MODE_NONE:
        return "None";
    case USB_MODE_SWITCH_DIVACON:
        return "Switch Divacon";
    case USB_MODE_SWITCH_HORIPAD:
        return "Switch Horipad";
    case USB_MODE_DUALSHOCK3:
        return "Dualshock 3";
    case USB_MODE_PS4_DIVACON:
        return "PS4 Diva";
    case USB_MODE_PS4_COMPAT:
        return "PS4 Compat";
    case USB_MODE_DUALSHOCK4:
        return "Dualshock 4";
    case USB_MODE_XBOX360:
        return "Xbox 360";
    case USB_MODE_PDLOADER:
        return "PDL Arcade";
    case USB_MODE_KEYBOARD:
        return "Keyboard";
    case USB_MODE_MIDI:
        return "MIDI";
    case USB_MODE_DEBUG:
        return "Debug";
    }

    return NULL;
}
