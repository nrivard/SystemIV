#pragma once

#define SPI_CS  (1<<5)

extern void SPIASSERT();
extern void SPIDEASSERT();

#define spi_cs_assert()     SPIASSERT()
#define spi_cs_deassert()   SPIDEASSERT()

// initialize the MFP for SPI
extern void spi_init(void);

// send a value to and receive a value from the connected SPI device
extern int spi_transfer(int);

static inline int spi_read(void) {
    return spi_transfer(0xFF);
}
