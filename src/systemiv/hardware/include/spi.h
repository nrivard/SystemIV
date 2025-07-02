#pragma once

#define spigate_data            ((volatile uint8_t * const)(0x800001))
#define spigate_cmd             ((volatile uint8_t * const)(0x800003))

typedef enum {
    spi_speed_fastest = 0,
    spi_speed_div_8,
    spi_speed_div_16,
    spi_speed_slowest
} spi_speed_t;

#ifndef SPIGATE

#define SPI_CS  (1<<5)

extern void SPIASSERT();
extern void SPIDEASSERT();
extern int SPITXC(int);
extern void SPIINIT();

#define spi_cs_assert()     SPIASSERT()
#define spi_cs_deassert()   SPIDEASSERT()
#define spi_init()          SPIINIT()
#define spi_transfer(data)  SPITXC(data)
#define spi_set_speed(int)  

#else

#include "spigate.h"

#define spi_cs_assert()         spigate_assert()
#define spi_cs_deassert()       spigate_deassert()
#define spi_init()              spigate_init()
#define spi_transfer(data)      spigate_transfer(data)
#define spi_set_speed(speed)    spigate_set_speed(speed)

#endif

static inline int spi_read(void) {
    return spi_transfer(0xFF);
}
