#include "ymf262.h"

#include "printf.h"

opl_error_t opl_init(opl_device_t *device) {
    opl_write(4, 0x60);
    opl_write(4, 0x80);

    volatile unsigned char result = opl_read();

    opl_write(2, 0xFF);
    opl_write(4, 0x21);

    // delay
    for (int i = 0xFFFF; i >= 0; i--) ;

    volatile unsigned char result2 = opl_read();

    // reset timer(s) again
    opl_write(4, 0x60);
    opl_write(4, 0x80);

    if (result != 0 && result2 != 0xC0) {
        return OPL_ERROR_NOT_FOUND;
    } else {
        device->type = result2 & 0x06 ? YMF262_OPL2 : YMF262_OPL3;
    }

    return OPL_ERROR_NONE;
}

void ymf262_play_test() {
    static uint8_t registers[] = {
        0x20, 0x40, 0x60, 0x80, 0xE0,
        0x23, 0x43, 0x63, 0x83, 0xE3, 
        0xC0
    };

    static uint8_t data[] = {
        0x01, 0x00, 0xFF, 0xFF, 0x00, 
        0x01, 0x00, 0xF5, 0x75, 0x00, 
        0x3C
    };

    static uint16_t notes[] = {
        0x159,
        0x183,
        0x1B3,
        0x1CC,
        0x205,
        0x244,
        0x28B,
        0x2D5
    };

    for (int i = 0; i < sizeof(registers); i++) {
       opl_write(registers[i], data[i]);
    }

    for (int i = 0; ; i = (i + 1) % (sizeof(notes) / sizeof(uint16_t))) {
        opl_write(0xA0, notes[i] & 0xFF);
        opl_write(0xB0, (notes[i] >> 8) | 0x30); // turn note on

        for (int delay = 0; delay < 0xFFFF; delay++);

        opl_write(0xB0, (notes[i] >> 8)); // turn note off
    }
}
