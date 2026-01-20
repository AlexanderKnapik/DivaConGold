#include "common/version.h"

#include <pico/stdlib.h>

#include <stdio.h>

int main()
{
    stdio_init_all();
    printf("Version: %u.%u.%u", version.major, version.minor, version.patch);

    while (true) {
        sleep_ms(1);
    }

    return 0;
}
