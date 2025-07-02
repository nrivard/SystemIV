#include "vgm.h"

#include "m68k.h"
#include "mfp.h"
#include "printf.h"
#include "stddef.h"
#include "string.h"
#include "somi.h"
#include "ymf262.h"

typedef enum {
    VGM_SAMPLE_IDLE = 0,
    VGM_SAMPLE_PROCESSED
} vgm_sample_status_t;

volatile static vgm_sample_status_t sample_status = VGM_SAMPLE_IDLE;
volatile static uint16_t sample_wait = 0;

__attribute__((interrupt)) void vgm_irq_hander() {
    sample_status = VGM_SAMPLE_IDLE;
    // *mfp_isra &= ~MFP_IRQ_TIMERS_AC;    // ack the irq
    asm volatile(
        // "move.b     #0, %0\n"                   // sample_status = VGM_SAMPLE_IDLE;
        "andi.b     #-33, %1\n"             // *mfp_isra &= ~MFP_IRQ_TIMERS_AC;
        : "=m" (sample_status), "=m" (*mfp_isra)
        :
        :
    );
} 

void vgm_init_timer() {
    m68k_vector_table->user[0x0D] = vgm_irq_hander;
    *mfp_tacr = (MFP_PRESCALE_4);   // 3686400 / 4  with 21 timer value == 43885 hz
    *mfp_tadr = 30;                 // timer value
    *mfp_iera |= MFP_IRQ_TIMERS_AC; // enable timer A
    *mfp_imra |= MFP_IRQ_TIMERS_AC; // unmask timer A
}

void vgm_destroy_timer() {
    *mfp_imra &= ~MFP_IRQ_TIMERS_AC;
    *mfp_iera &= ~MFP_IRQ_TIMERS_AC;
}

void vgm_play() {
    vgm_header_t *header = (vgm_header_t *)somi_vgm;
    if (strncmp(header->ident, "Vgm ", 4)) {
        printf("Not a vgm file!");
        return;
    }

    uint32_t clk = 0;
    if ((clk = swap_endian32(header->ymf262))) {
        printf("Found OPL3 file!\n");
    } else if ((clk = swap_endian32(header->ym3812))) {
        printf("Found OPL2 file!\n");
    }
    printf("clk: %l\n", clk);

    uint32_t eof = swap_endian32(header->eof) + offsetof(vgm_header_t, eof);
    printf("eof: %l\n", eof);
    
    uint32_t vgmOffset = swap_endian32(header->data) + offsetof(vgm_header_t, data);
    printf("vgm data: %l\n", vgmOffset);

    vgm_init_timer();

    sample_status = VGM_SAMPLE_IDLE;
    sample_wait = 0;
    for (uint32_t idx = vgmOffset; vgmOffset < eof; ) {
        while (sample_status) ; // wait until sample has been processed
        sample_status = VGM_SAMPLE_PROCESSED;

        if (sample_wait) {
            sample_wait--;
            continue;
        }

        uint8_t cmd = somi_vgm[idx];
        switch (cmd) {
            case 0x61:
                sample_wait = somi_vgm[idx + 1] | (somi_vgm[idx + 2] << 8); // little endian
                idx += 2;
                break;

            case 0x5A:
                uint8_t reg = somi_vgm[++idx];
                uint8_t val = somi_vgm[++idx];
                opl_write(reg, val);
                break;

            default:
                printf("cmd: %d\n", cmd);
        }

        idx++;
    }

    vgm_destroy_timer();
}
