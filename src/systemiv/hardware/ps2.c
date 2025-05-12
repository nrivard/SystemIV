#include "ps2.h"

#define PS2_DAT                 (*((volatile uint8_t * const)(0x800001)))
#define PS2_STAT                (*((volatile uint8_t * const)(0x800003)))

uint8_t ps2_data() {
    return PS2_DAT;
}

void ps2_set_data(uint8_t data) {
    PS2_DAT = data;
}

uint8_t ps2_status() {
    return PS2_STAT;
}

void ps2_set_status(uint8_t status) {
    PS2_STAT = status;
}
