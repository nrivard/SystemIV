#include "serial.h"

#include <ctype.h>
#include <mfp.h>

#define INIT()      mfp_init()
#define GETC()      mfp_receive()
#define PUTC(c)     mfp_send(c)

unsigned int serial_dev = 0;

static char const hex[] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

oserr_t serial_read_block(unsigned int blk, char *buffer, void *ctx) {
    buffer[0] = GETC();
    return NOERR;
}

oserr_t serial_read_block_n(unsigned int blk, unsigned int n, char *buffer, void *ctx) {
    for (int i = 0; i < n; i++) {
        buffer[i] = GETC();
    }

    return NOERR;
}

oserr_t serial_write_block(unsigned int blk, char const *buffer, void *ctx) {
    PUTC(buffer[0]);
    return NOERR;
}

oserr_t serial_write_block_n(unsigned int blk, unsigned int n, char const *buffer, void *ctx) {
    for (int i = 0; i < n; i++) {
        PUTC(buffer[i]);
    }

    return NOERR;
}

void serial_put(char c) {
    device_t dev;
    // TODO: for now there is no point in panicing. it will
    // just try to serial print lmao
    device_get(serial_dev, &dev);
    dev.write_blk(0, &c, NULL);
}

oserr_t serial_init(device_t *device) {
    INIT();

    device->blk_size = 1;
    device->read_blk = serial_read_block;
    device->read_blk_n = serial_read_block_n;
    device->write_blk = serial_write_block;
    device->write_blk_n = serial_write_block_n;

    return NOERR;
}

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
