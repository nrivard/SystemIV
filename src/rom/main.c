#include <stdint.h>
#include "mfp.h"
#include "shellac.h"
#include "serial.h"

extern uint16_t _bss_start, _bss_end, _data_start, _data_end, _data_load_start, _data_load_end;

/*
ASCII Banner. Should look like:
   _____            __                     ___    __
  / ___/__  _______/ /____  ____ ___      /=/ |  / /
  \__ \/ / / / ___/ __/ _ \/ __ `__ \    /=/| | / / 
 ___/ / /_/ (__  ) /_/  __/ / / / / /   /=/ | |/ /  
/____/\__, /____/\__/\___/_/ /_/ /_/   /=/  |___/   
     \____/
*/
static const char banner[] = \
"\r\n" \
"   _____            __                     ___    __\r\n" \
"  / ___/__  _______/ /____  ____ ___      /=/ |  / /\r\n" \
"  \\__ \\/ / / / ___/ __/ _ \\/ __ `__ \\    /=/| | / /\r\n" \
" ___/ / /_/ (__  ) /_/  __/ / / / / /   /=/ | |/ /\r\n" \
"/____/\\__, /____/\\__/\\___/_/ /_/ /_/   /=/  |___/\r\n" \
"     \\____/\r\n";

__attribute__ ((__noreturn__)) void sysmain() {
    // initialize DATA
    for (uint16_t *data = &_data_start, *load = &_data_load_start; load < &_data_load_end; *data++ = *load++);

    // zero out BSS
    for (uint16_t *bss = &_bss_start; bss < &_bss_end; *bss++ = 0);

    serial_put_string(banner);

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
