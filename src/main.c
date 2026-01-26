#include "log/log.h"
#include "usb/usb.h"

#include <pico/stdlib.h>


int main()
{
    set_sys_clock_khz(120000, true);

    buttons_init();
    usb_open();

    while (true) {
    }

    return 0;
}
