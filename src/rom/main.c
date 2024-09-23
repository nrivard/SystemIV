#include <stdint.h>
#include "mfp.h"
#include "shellac.h"
#include "serial.h"
#include "sdcard.h"
// #include "fat.h"

extern uint16_t _bss_start, _bss_end, _data_start, _data_end, _data_load_start, _data_load_end;
extern uint8_t  _bootloader_start;

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
    // initialize DATA
    for (uint16_t *data = &_data_start, *load = &_data_load_start; load < &_data_load_end; *data++ = *load++);

    // zero out BSS
    for (uint16_t *bss = &_bss_start; bss < &_bss_end; *bss++ = 0);

    serial_put_string(banner);

    serial_put_string("Searching for bootable media...");

    sdcard_device_t device;
    sdcard_error_t error = sdcard_init(&device);
    if (error != SDCARD_NOERR || device.status != SDCARD_STATUS_READY) {
        serial_put_string("No bootable media found.\r\n");
        serial_put_string("Error: ");
        serial_put_hex(error);
        serial_put_string("\r\nDevice status: ");
        serial_put_hex(device.status);
        serial_put_string("\r\n");
        goto Shell;
    }

    // fetch MBR
    uint8_t *mbr = &_bootloader_start;
    sdcard_data_token_t token;
    error = sdcard_read_block(0, mbr, &token);
    if (error != SDCARD_NOERR) {
        serial_put_string("MBR could not be read.\r\n");
        serial_put_string("Error: ");
        serial_put_hex(error);
        serial_put_string("\r\nToken: ");
        serial_put_hex(token);
        serial_put_string("\r\n");
        goto Shell;
    }

    serial_put_string("found.\r\n");

    // DEBUG: just print what we get in the first sector
    // for (int i = 0; i < FAT_SECTOR_SIZE; i++) {
    //     if (i != 0 && (i % 8) == 0) {
    //         serial_put_string("\r\n");
    //     }
    //     serial_put_hex(mbr[i]);
    //     serial_put(' ');
    // }
    
    serial_put_string("\r\nExecuting boot disk\r\n");

    // execute mbr boot code! if it returns we got an error
    error = ((int (*)(void))mbr)();
    serial_put_string("FATAL ERROR: ");
    serial_put_hex(error);

Shell:
    serial_put_string("\r\nLaunching Shellac.\r\n");
    shellac_main();

    // DEBUG: just print what we get in the first sector
    // char *debug = (char *)(mbr + FAT_SECTOR_SIZE);
    // for (int i = 0; i < FAT_SECTOR_SIZE + 20; i++) {
    //     if (i != 0 && (i % 8) == 0) {
    //         serial_put_string("\r\n");
    //     }
    //     serial_put_hex(debug[i]);
    //     serial_put(' ');
    // }

    // VolumeInfo *volume = (VolumeInfo *)(mbr + 1024);
    // serial_put_string("ID   :");
    // serial_put_long(volume->id);
    // serial_put_string("\r\FAT  :");
    // serial_put_long(volume->fat);
    // serial_put_string("\r\nROOT :");
    // serial_put_long(volume->root);
    // serial_put_string("\r\nCLSTR:");
    // serial_put_long(volume->cluster);
    // serial_put_string("\r\nTYPE :");
    // serial_put_hex(volume->type);
    // serial_put_string("\r\nSECS :");
    // serial_put_hex(volume->secs);
    // serial_put_string("\r\n");

    HALT();
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
