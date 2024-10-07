#include "serial.h"

static char const hex[] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

void serial_put_string(const char *str) {
    register char c;
    while ((c = *str++)) {
        serial_put(c);
    }
}

void serial_put_long(const unsigned long d) {
    register unsigned long val = d;
    for (int i = (sizeof(d) * 2) - 1; i >= 0; i--) {
        register char c = val >> (i * 4);
        serial_put(hex[c & 0xF]);
    }
}

void serial_put_hex(const char c) {
    // have to promote to an int or weird stuff happens...
    register int b = c >> 4;
    serial_put(hex[b & 0xF]);
    serial_put(hex[c & 0xF]);
}
