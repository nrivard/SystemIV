#include <stdint.h>

#include "irq.h"
#include "kalloc.h"
#include "params.h"
#include "printf.h"
#include "sdcard.h"
#include "serial.h"
#include "vm.h"

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
    // bring serial port up first
    serial_init();
    serial_put_string(banner);

    kinit();

    // kvminit();

    // serial_put_string("Searching for disk...");
    // sdcard_device_t disk;
    // if (sdcard_init(&disk) != SDCARD_NOERR || disk.status != SDCARD_STATUS_READY || disk.type == SDCARD_DEVICE_NONE) {
    //     serial_put_string("not found\r\n");
    // } else {
    //     serial_put_string("found ");
    //     serial_put_string(sdcard_device_type_msg(disk.type));
    //     serial_put_string("\r\n");
    // }

    // turn interrupts back on
    irq_on();

    HALT();
}
