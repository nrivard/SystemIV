#include <stdint.h>
#include <time.h>
#include "mfp.h"
#include "shellac.h"
#include "serial.h"
#include "sdcard.h"
#include "xmodem.h"

extern uint16_t _bss_start, _bss_end, _data_start, _data_end, _data_load_start, _data_load_end;
extern uint8_t  _bootloader_start, _os_start;

uint8_t boot_from_disk(void);
uint8_t boot_from_serial(void);

static inline __attribute__((__noreturn__)) void HALT() {
    while(1);
}

void fatal_error(uint8_t error) {
    serial_put_string("FATAL ERROR: ");
    serial_put_hex(error);
    serial_put_string("\n");
    shellac_main();
} 

__attribute__ ((__noreturn__)) void sysmain() {
    // initialize DATA
    for (uint16_t *data = &_data_start, *load = &_data_load_start; load < &_data_load_end; *data++ = *load++);

    // zero out BSS
    for (uint16_t *bss = &_bss_start; bss < &_bss_end; *bss++ = 0);

    serial_put_string("Sixty n8k ROM v1\n");
    serial_put_string("Searching for bootable media...");
    
    // try to boot from disk first
    boot_from_disk();

    // if we are here, booting from disk failed. try to boot from serial
    boot_from_serial();

    HALT();
}

uint8_t boot_from_disk() {
    sdcard_device_t device;
    sdcard_error_t error = sdcard_init(&device);
    if (error != SDCARD_NOERR || device.status != SDCARD_STATUS_READY) {
        serial_put_string("No disk found.\n");
        return error;
    }

    serial_put_string("disk found.\nExecuting boot disk\n");

    // fetch MBR
    uint8_t *mbr = &_bootloader_start;
    sdcard_data_token_t token;
    error = sdcard_read_block(0, mbr, &token);
    if (error != SDCARD_NOERR) {
        serial_put_string("MBR could not be read.\n");
        serial_put_string("Error: ");
        serial_put_hex(error);
        serial_put_string("\nToken: ");
        serial_put_hex(token);
        serial_put_string("\n");
        return error;
    }

    // execute mbr boot code! if it returns we got an error
    error = ((int (*)(void))mbr)();

    fatal_error(error);
    return error;
}

uint8_t boot_from_serial() {
    serial_put_string("Waiting to receive via serial...");

    // place OS at desired location and cap it at 32k, same requirement as the disk bootloader version
    uint8_t error = xmodem_recv(&_os_start, 0x8000);

    // if we don't delay we can overwhelm the MFP
    delay(10);

    if (error != XMODEM_NOERR) {
        serial_put_string("failed.\n");
        fatal_error(error);
    }

    // execute the received OS
    error = ((int (*)(void))&_os_start)();

    // if we got here, the OS failed
    fatal_error(error);
    return error;
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
