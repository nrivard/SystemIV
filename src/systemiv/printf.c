#include <stdarg.h>

#include "params.h"
#include "serial.h"
#include "stdint.h"

#include "printf.h"

void printf(const char *fmt, ...) {
    // params on stack on m68k are always 4 bytes
    uint32_t *params = ((uint32_t *)&fmt + 1);

    char c;
    while ((c = *fmt++)) {
        if (c != '%') {
            serial_put(c);
            continue;
        }

        // specifier
        c = *fmt++;
        switch (c) {
            case 'd':
                serial_put_hex(*params++);
                break;
            case 'l':
                serial_put_long(*params++);
                break;
            case 'p':
                serial_put('0');
                serial_put('x');
                serial_put_long(*params++);
                break;
            case 's':
                serial_put_string((char *)*params++);
                break;
            default:
                serial_put('%');
                serial_put(c);
        }
    }
}

void panic(const char *s) {
    printf("PANIC: %s\r\n", s);
    HALT();
}
