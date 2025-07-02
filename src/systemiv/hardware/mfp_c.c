#include <stdint.h>

#include "mfp.h"

#define MFP_IRQ_VCTR_BASE       0x40
#define MFP_IRQ_VCTR_TIMER_C    ((MFP_IRQ_VCTR_BASE + 0x05) * 4)
#define MFP_IRQ_VCTR_SOFTWARE   (0x08)

#define MFP_UCR_STP_BIT_1   (1<<3)
#define MFP_UCR_DIV_16      (1<<7)
#define MFP_TSR_EN          (1<<0)
#define MFP_TSR_HIGH        (1<<2)
#define MFP_TSR_BUF_EMPTY   (1<<7)
#define MFP_RSR_EN          (1<<0)

void mfp_init() {
    // all but bit 0 as output
    *mfp_ddr = 0xFE;

    // pre-scalars for timers C & D
    *mfp_tcdcr = (MFP_PRESCALE_200 << 4) | (MFP_PRESCALE_4);

    // timer c value for 100hz after prescaling (note: this is edge triggered!)
    *mfp_tcdr = 0xB8;

    // timer d value for 9600 baud rate after prescaling
    *mfp_tddr = 0x03;

    // divide by 16, async, 8N1
    *mfp_ucr = MFP_UCR_DIV_16 | MFP_UCR_STP_BIT_1;

    // set line default high, enable transmitter
    *mfp_tsr = MFP_TSR_HIGH | MFP_TSR_EN;

    // enable receiver
    *mfp_rsr = MFP_RSR_EN;

    // set IRQ vector base and turn on software end-of-interrupt mode
    *mfp_vr = MFP_IRQ_VCTR_BASE | MFP_IRQ_VCTR_SOFTWARE;

    // enable timer c interrupt (initially masked)
    *mfp_ierb |= MFP_IRQ_TIMERS_AC;
}

void mfp_send(char c) {
    while (!(*mfp_tsr & 0x80));
    *mfp_udr = c;
}

char mfp_receive() {
    while (!(*mfp_rsr & 0x80));
    return *mfp_udr;
}

int mfp_byte_avail() {
    return *mfp_rsr & 0x80;
}

void mfp_irq_ack_timerc() {
    *mfp_isrb &= ~MFP_IRQ_TIMERS_AC;
}
