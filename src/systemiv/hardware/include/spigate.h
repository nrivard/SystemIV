#pragma once

#include <stdint.h>

#define spigate_data            ((volatile uint8_t * const)(0x800001))
#define spigate_cmd             ((volatile uint8_t * const)(0x800003))

#define SPIGATE_CMD_CLK_SEL     0b00000011
#define SPIGATE_CMD_DEV_SEL     0b00001100
#define SPIGATE_CMD_DEN         0b00010000
#define SPIGATE_CMD_IEN         0b00100000
#define SPIGATE_CMD_BUSY        0b01000000
#define SPIGATE_CMD_ITC         0b10000000

typedef enum {
    spigate_speed_div_2 = 0,
    spigate_speed_div_8,
    spigate_speed_div_16,
    spigate_speed_div_32
} spigate_speed_t;

// initialize spi gate for use (lowest speed, all devices deselected, no interrupts)
void spigate_init(void);

// blocking call that initiates an SPI transfer and returns the result
int spigate_transfer(int);

// assert or deassert /CS line
void spigate_assert(void);
void spigate_deassert(void);

// turn on fastest speed
void spigate_set_speed(spigate_speed_t);
