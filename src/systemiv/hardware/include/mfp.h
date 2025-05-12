#pragma once

// extern uint8_t MFP_BASE, ;
// volatile uint8_t *mfp_port = (uint8_t *)(&MFP_BASE);



void mfp_init(void);
int  mfp_byte_avail(void);
char mfp_receive(void);
void mfp_send(char c);

void mfp_irq_ack_timerc(void);
