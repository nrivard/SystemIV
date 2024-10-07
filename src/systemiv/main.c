#include <stdint.h>
#include "sdcard.h"
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

static inline __attribute__((__noreturn__)) void HALT() {
    while(1);
}

__attribute__ ((__noreturn__)) void sysmain() {
    // bring serial port up first
    serial_init();
    serial_put_string(banner);

    sdcard_device_t disk;
    if (sdcard_init(&disk) != SDCARD_NOERR) {
        serial_put_string("No disk found\r\n");
        HALT();
    }

    
}
