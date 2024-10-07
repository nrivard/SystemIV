#pragma once

extern void mfp_init(void);
extern int  mfp_byte_avail(void);
extern int  mfp_receive(void);
extern void mfp_send(int c);
