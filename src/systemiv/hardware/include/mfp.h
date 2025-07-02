#pragma once

// extern uint8_t MFP_BASE, ;
// volatile uint8_t *mfp_port = (uint8_t *)(&MFP_BASE);

#define MFP_REG(off)    ((volatile uint8_t * const)(0xE00001 + (off << 1)))

#define mfp_gpip            MFP_REG(0x00)
#define mfp_ddr             MFP_REG(0x02)
#define mfp_iera            MFP_REG(0x03)
#define mfp_ierb            MFP_REG(0x04)
#define mfp_isra            MFP_REG(0x07)
#define mfp_isrb            MFP_REG(0x08)
#define mfp_imra            MFP_REG(0x09)
#define mfp_imrb            MFP_REG(0x0A)
#define mfp_vr              MFP_REG(0x0B)
#define mfp_tacr            MFP_REG(0x0C)
#define mfp_tbcr            MFP_REG(0x0D)
#define mfp_tcdcr           MFP_REG(0x0E)
#define mfp_tadr            MFP_REG(0x0F)
#define mfp_tbdr            MFP_REG(0x10)
#define mfp_tcdr            MFP_REG(0x11)
#define mfp_tddr            MFP_REG(0x12)
#define mfp_ucr             MFP_REG(0x14)
#define mfp_rsr             MFP_REG(0x15)
#define mfp_tsr             MFP_REG(0x16)
#define mfp_udr             MFP_REG(0x17)

#define MFP_PRESCALE_4      (0b001)
#define MFP_PRESCALE_10     (0b010)
#define MFP_PRESCALE_16     (0b011)
#define MFP_PRESCALE_50     (0b100)
#define MFP_PRESCALE_64     (0b101)
#define MFP_PRESCALE_100    (0b110)
#define MFP_PRESCALE_200    (0b111)

#define MFP_IRQ_TIMERS_AC      0x20

void mfp_init(void);
int  mfp_byte_avail(void);
char mfp_receive(void);
void mfp_send(char c);

void mfp_irq_ack_timerc(void);
