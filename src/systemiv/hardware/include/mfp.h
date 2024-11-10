#pragma once

// extern uint8_t MFP_BASE, ;
// volatile uint8_t *mfp_port = (uint8_t *)(&MFP_BASE);

extern void mfp_init(void);
extern int  mfp_byte_avail(void);
extern int  mfp_receive(void);
extern void mfp_send(int c);
