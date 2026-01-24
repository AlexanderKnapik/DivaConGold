#include "log/log.h"

#include <pico/stdlib.h>

#include <stdio.h>

int main()
{
    log_open(NULL);

    while (true) {
        sleep_ms(1);
    }

    return 0;
}
