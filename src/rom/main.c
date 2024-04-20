#include <stdint.h>
#include "mfp.h"
#include "shellac.h"

extern uint16_t _bss_start, _bss_end, _data_start, _data_end, _data_load_start, _data_load_end;

__attribute__ ((__noreturn__)) void sysmain() {
    // initialize DATA
    for (uint16_t *data = &_data_start, *load = &_data_load_start; load < &_data_load_end; *data++ = *load++);

    // zero out BSS
    for (uint16_t *bss = &_bss_start; bss < &_bss_end; *bss++ = 0);

    shellac_main();

    while (1);
}

char serial_get(void) {
    return mfp_receive();
}

void serial_put(char c) {
    mfp_send(c);
}

int serial_byte_avail() {
    return mfp_byte_avail();
}
