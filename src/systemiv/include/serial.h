#pragma once

/// START OF SIXTY-N8K BLOCK ///
#include "mfp.h"

#define     serial_init()           mfp_init()
#define     serial_get()            mfp_receive()
#define     serial_put(x)           mfp_send(x)
#define     serial_byte_avail()     mfp_byte_avail()
/// END OF SIXTY-N8K BLOCK ///

void serial_put_string(const char *);
void serial_put_hex(const char c);
void serial_put_long(const unsigned long d);
