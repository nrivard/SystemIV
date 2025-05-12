#pragma once

#include <stdint.h>

#define PS2_STATUS_IE           0b10000000
#define PS2_STATUS_ERR          0b01000000
#define PS2_STATUS_BUSY         0b00100000
#define PS2_STATUS_RECV         0b00010000

uint8_t ps2_data(void);
void    ps2_set_data(uint8_t);

uint8_t ps2_status(void);
void    ps2_set_status(uint8_t status);
