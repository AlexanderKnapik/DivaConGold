/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "usb_descriptors.h"

#include "usb/keyboard.h"

#include <bsp/board_api.h>
#include <tusb.h>

#include <stdint.h>

char const *string_desc_arr[] = {
    (const char[]){0x09, 0x04}, // 0: is supported language is English (0x0409)
    "TinyUSB",                  // 1: Manufacturer
    "TinyUSB Device",           // 2: Product
    NULL,                       // 3: Serials will use unique ID if possible
};

/* TODO: Remove magic numbers */
static uint16_t desc_str[32 + 1] = {};

/*
 * Invoked when received GET DEVICE DESCRIPTOR
 * Application return pointer to descriptor
 */
uint8_t const *tud_descriptor_device_cb(void)
{
    return (uint8_t const *)&keyboard_desc_device;
}

/*
 * Invoked when received GET HID REPORT DESCRIPTOR
 * Application return pointer to descriptor
 * Descriptor contents must exist long enough for transfer to complete
 */
uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance)
{
    (void)instance;
    return keyboard_desc_hid_report;
}

/*
 * Invoked when received GET CONFIGURATION DESCRIPTOR
 * Application return pointer to descriptor
 * Descriptor contents must exist long enough for transfer to complete
 */
uint8_t const *tud_descriptor_configuration_cb(uint8_t index)
{
    (void)index; // for multiple configurations
    return keyboard_desc_configuration;
}

/*
 * Invoked when received GET STRING DESCRIPTOR request.
 *
 * Application return pointer to descriptor, whose contents must exist long
 * enough for transfer to complete.
 */
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    (void)langid;
    size_t chr_count = 0;

    switch (index) {
    case USB_STR_ID_LANGID:
        memcpy(&desc_str[1], string_desc_arr[0], 2);
        chr_count = 1;
        break;

    case USB_STR_ID_SERIAL:
        chr_count = board_usb_get_serial(desc_str + 1, 32);
        break;

    default:
        /*
         * Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
         * https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors
         */
        if (!(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0]))) {
            return NULL;
        }

        /* Cap at max char */
        size_t const max_count = (sizeof(desc_str) / sizeof(desc_str[0])) - 1; // -1 for string type

        const char *str = string_desc_arr[index];
        chr_count = strlen(str);

        if (chr_count > max_count) {
            chr_count = max_count;
        }

        /* Convert ASCII string into UTF-16 */
        for (size_t idx = 0; idx < chr_count; idx++) {
            desc_str[idx + 1] = str[idx];
        }
        break;
    }

    /*
     * first byte is length (including header), second byte is string type
     * desc_str[0] = (uint16_t)((TUSBdesc_strING << 8) | (2 * chr_count + 2));
     */
    desc_str[0] = (uint16_t)((TUSB_DESC_STRING << 8) | ((chr_count << 1) + 2));

    return desc_str;
}
