#include <stdint.h>

#include "irq.h"
#include "kalloc.h"
#include "params.h"
#include "printf.h"
#include "proc.h"
#include "sdcard.h"
#include "serial.h"
#include "vm.h"

#include "ps2.h"

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
"\n" \
"   _____            __                     ___    __\n" \
"  / ___/__  _______/ /____  ____ ___      /=/ |  / /\n" \
"  \\__ \\/ / / / ___/ __/ _ \\/ __ `__ \\    /=/| | / /\n" \
" ___/ / /_/ (__  ) /_/  __/ / / / / /   /=/ | |/ /\n" \
"/____/\\__, /____/\\__/\\___/_/ /_/ /_/   /=/  |___/\n" \
"     \\____/\n";

static uint8_t system_jiffies = 0;
static uint32_t system_uptime = 0;

char buffer[128];
char *start = buffer;
char *end = buffer;

__attribute__((interrupt)) void irq_timer() {
    system_jiffies++;
    if (system_jiffies == 100) {
        system_jiffies = 0;
        system_uptime++;
    }
    mfp_irq_ack_timerc();
}

__attribute__((interrupt)) void ps2_recv() {
    printf("data: %d\n", ps2_data());
}

__attribute__ ((__noreturn__)) void sysmain() {
    // initialize DATA
    for (uint16_t *data = &_data_start, *load = &_data_load_start; load < &_data_load_end; *data++ = *load++);

    // zero out BSS
    for (uint16_t *bss = &_bss_start; bss < &_bss_end; *bss++ = 0);

    // bring serial port up first
    serial_init();
    printf("%s", banner);

    kinit();

    m68k_vector_table->user[5] = irq_timer;
    m68k_vector_table->auto_vec[5] = ps2_recv;

    // start init process
    // procs[0].pid = 0;
    // procs[0].status = PROC_RUNNABLE;

    // kvminit();

    // serial_put_string("Searching for disk...");
    // sdcard_device_t disk;
    // if (sdcard_init(&disk) != SDCARD_NOERR || disk.status != SDCARD_STATUS_READY || disk.type == SDCARD_DEVICE_NONE) {
    //     serial_put_string("not found\n");
    // } else {
    //     serial_put_string("found ");
    //     serial_put_string(sdcard_device_type_msg(disk.type));
    //     serial_put_string("\n");
    // }

    // turn interrupts back on
    irq_on();

    ps2_set_status(PS2_STATUS_IE);

    // DEBUG
    // while (1) {
    //     // char stat = ;
    //     // printf("status: %d", stat);
    //     // if (ps2_status() & PS2_STATUS_RECV) {
    //     //     printf("recv: %d\n", ps2_data());
    //     // }
    //     // char data;
    //     // if ((data = ps2_data())) {
    //     //     printf("data: %d", data);
    //     // }
    // }

    HALT();
}
