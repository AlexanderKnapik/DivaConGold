#ifndef USB_H_
#define USB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common/error.h"

enum error usb_open(void);
enum error usb_task(void);

#ifdef __cplusplus
}
#endif

#endif // USB_H_
